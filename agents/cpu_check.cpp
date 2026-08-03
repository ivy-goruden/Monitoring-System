#include "cpu_check.hpp"
#include <thread>

using namespace std::chrono_literals;
using namespace s21;

Cpu_Check::Cpu_Check(){
    metrics_ = {{"cpu_usage", 0.0f}};
    warnings_ = {};
    timer_ = 1s;
    isRunning_.store(true);
    crit_value_.store(50.0);
    crit_sign_.store(LESS);
}

Cpu_Check::~Cpu_Check(){
    isRunning_.store(false);
}

void Cpu_Check::get_cpu_times(size_t &idle_time, size_t &total_time) {
    std::ifstream file("/proc/stat");
    std::string cpu;
    file >> cpu; // Consume "cpu" anchor
    
    std::vector<size_t> times(10);
    for (size_t i = 0; i < 10; ++i) {
        file >> times[i];
    }
    
    idle_time = times[3] + times[4]; // idle + iowait
    total_time = std::accumulate(times.begin(), times.end(), 0ULL);
}

void Cpu_Check::checkCritical(Sign sign, float value, std::string metric){
    if (sign == LESS && metrics_[metric] < value) {
        createWarning("CPU Usage is below " + std::to_string(value) + "%");
    } else if (sign == GREATER && metrics_[metric] > value) {
        createWarning("CPU Usage is above " + std::to_string(value) + "%");
    } else if (sign == EQUAL && metrics_[metric] == value) {
        createWarning("CPU Usage is equal to " + std::to_string(value) + "%");
    } else if (sign == LESS_EQUAL && metrics_[metric] <= value) {
        createWarning("CPU Usage is below or equal to " + std::to_string(value) + "%");
    } else if (sign == GREATER_EQUAL && metrics_[metric] >= value) {
        createWarning("CPU Usage is above or equal to " + std::to_string(value) + "%");
    }
}

std::vector<std::string> Cpu_Check::getWarnings(){
    std::lock_guard<std::mutex> lock(warning_mutex_);
    std::vector<std::string> warnings_copy = warnings_;
    warnings_.clear();
    return warnings_copy;
}

void Cpu_Check::execute(){
    isRunning_.store(true);
    mainLoop();
}

extern "C"{
    Cpu_Check cpu_check;
    void execute(){
        cpu_check.execute();
    }
    void start(){
        cpu_check.start();
    }
    void stop(){
        cpu_check.stop();
    }
    void kill(){
        cpu_check.kill();
    }
    void getMetrics(std::map<std::string, float> &metrics){
        cpu_check.getMetrics(metrics);
    }
    std::vector<std::string> getWarnings(){
        return cpu_check.getWarnings();
    }
    void update(std::string JsonConfig){
        cpu_check.update(JsonConfig);
    }

}

int Cpu_Check::mainLoop() {
    size_t idle1, total1, idle2, total2;
    
    while(isRunning_.load()) {
        if (isPaused_.load()) {
            std::this_thread::sleep_for(1s);
            continue;
        }
        get_cpu_times(idle1, total1);
        std::this_thread::sleep_for(1s); // Wait 1 second interval
        get_cpu_times(idle2, total2);
        
        const double active_delta = (total2 - total1) - (idle2 - idle1);
        const double total_delta = total2 - total1;
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_["cpu_usage"] = (active_delta / total_delta) * 100.0;
        mutex_.unlock();
        checkCritical(LESS, crit_value_.load(), "cpu_usage");
        std::this_thread::sleep_for(timer_);
    }
    return 0;
}

void Cpu_Check::getMetrics(std::map<std::string, float> &metrics){
    std::lock_guard<std::mutex> lock(mutex_);
    metrics = metrics_;
}

void Cpu_Check::update(std::string JsonConfig){
    nlohmann::json config = nlohmann::json::parse(JsonConfig);
    crit_value_.store(config["crit_value"]["cpu_usage"][0].get<float>());
    timer_ = std::chrono::seconds(config["timer"].get<int>());
    crit_sign_.store(static_cast<Sign>(config["crit_sign"]["cpu_usage"][1].get<int>()));
}

void Cpu_Check::createWarning(std::string warning){
    std::lock_guard<std::mutex> lock(warning_mutex_);
    warnings_.push_back(warning);
}
