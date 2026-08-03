#include "swap_check.hpp"
#include <sstream>
#include <thread>

using namespace std::chrono_literals;
using namespace s21;

Swap_Check::Swap_Check() {
    metrics_ = {{"used_swap", 0.0f}};
    warnings_ = {};
    timer_ = 3s;
    isRunning_.store(true);
    crit_value_.store(0.0f);
    crit_sign_.store(GREATER);
}

Swap_Check::~Swap_Check() {
    isRunning_.store(false);
}

void Swap_Check::get_swap_metrics() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long total_kb = 0;
    long free_kb = 0;

    while (std::getline(meminfo, line)) {
        if (line.rfind("SwapTotal:", 0) == 0) {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> total_kb;
        } else if (line.rfind("SwapFree:", 0) == 0) {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> free_kb;
        }
    }

    const float used_swap_gb = static_cast<float>(total_kb - free_kb) / (1024.0f * 1024.0f);
    std::lock_guard<std::mutex> lock(metric_mutex_);
    metrics_["used_swap"] = used_swap_gb < 0.0f ? 0.0f : used_swap_gb;
}

void Swap_Check::checkCritical(Sign sign, float value, std::string metric_name) {
    float current_value = 0.0f;
    {
        std::lock_guard<std::mutex> lock(metric_mutex_);
        current_value = metrics_[metric_name];
    }

    if (sign == LESS && current_value < value) {
        createWarning(metric_name + " is below " + std::to_string(value) + " GB");
    } else if (sign == GREATER && current_value > value) {
        createWarning(metric_name + " is above " + std::to_string(value) + " GB");
    } else if (sign == EQUAL && current_value == value) {
        createWarning(metric_name + " is equal to " + std::to_string(value) + " GB");
    } else if (sign == LESS_EQUAL && current_value <= value) {
        createWarning(metric_name + " is below or equal to " + std::to_string(value) + " GB");
    } else if (sign == GREATER_EQUAL && current_value >= value) {
        createWarning(metric_name + " is above or equal to " + std::to_string(value) + " GB");
    }
}

void Swap_Check::createWarning(const std::string& warning) {
    std::lock_guard<std::mutex> lock(warning_mutex_);
    warnings_.push_back(warning);
}

std::vector<std::string> Swap_Check::getWarnings() {
    std::lock_guard<std::mutex> lock(warning_mutex_);
    std::vector<std::string> warnings_copy = warnings_;
    warnings_.clear();
    return warnings_copy;
}

void Swap_Check::getMetrics(std::map<std::string, float> &metrics) {
    std::lock_guard<std::mutex> lock(metric_mutex_);
    metrics = metrics_;
}

void Swap_Check::execute() {
    isRunning_.store(true);
    mainLoop();
}

int Swap_Check::mainLoop() {
    while (isRunning_.load()) {
        if (isPaused_.load()) {
            std::this_thread::sleep_for(1s);
            continue;
        }

        get_swap_metrics();
        checkCritical(crit_sign_.load(), crit_value_.load(), "used_swap");
        std::this_thread::sleep_for(timer_);
    }
    return 0;
}

void Swap_Check::update(std::string JsonConfig) {
    try {
        nlohmann::json config = nlohmann::json::parse(JsonConfig);
        if (config.contains("crit_value") && config["crit_value"].contains("used_swap")) {
            crit_value_.store(config["crit_value"]["used_swap"].get<float>());
        }
        if (config.contains("timer")) {
            timer_ = std::chrono::seconds(config["timer"].get<int>());
        }
        if (config.contains("crit_sign") && config["crit_sign"].contains("used_swap")) {
            crit_sign_.store(static_cast<Sign>(config["crit_sign"]["used_swap"].get<int>()));
        }
    } catch (const std::exception& e) {
        createWarning(std::string("config parse error: ") + e.what());
    }
}

extern "C" {
    Swap_Check swap_check;

    void execute() {
        swap_check.execute();
    }

    void start() {
        swap_check.start();
    }

    void stop() {
        swap_check.stop();
    }

    void kill() {
        swap_check.kill();
    }

    void getMetrics(std::map<std::string, float> &metrics) {
        swap_check.getMetrics(metrics);
    }

    std::vector<std::string> getWarnings() {
        return swap_check.getWarnings();
    }

    void update(std::string JsonConfig) {
        swap_check.update(JsonConfig);
    }
}
