#include "mem_check.hpp"
#include <sys/statvfs.h>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
namespace s21{
extern "C"{
    Mem_Check mem_check;
    void execute(){
        mem_check.execute();
    }
    void start(){
        mem_check.start();
    }
    void stop(){
        mem_check.stop();
    }
    void kill(){
        mem_check.kill();
    }
    void getMetrics(std::map<std::string, float> &metrics){
        mem_check.getMetrics(metrics);
    }
    std::vector<std::string> getWarnings(){
        return mem_check.getWarnings();
    }
    void update(std::string JsonConfig){
        mem_check.update(JsonConfig);
    }

}

void Mem_Check::get_mem_metrics(){
     std::ifstream meminfo("/proc/meminfo");
        std::string line;
        long total_kB = 0, avail_kB = 0;
        while (std::getline(meminfo, line)) {
            if (line.compare(0, 8, "MemTotal") == 0) {
                std::istringstream iss(line);
                std::string key;
                long value;
                iss >> key >> value;
                total_kB = value;
            } else if (line.compare(0, 13, "MemAvailable") == 0) {
                std::istringstream iss(line);
                std::string key;
                long value;
                iss >> key >> value;
                avail_kB = value;
            }
        }
        ram_total = total_kB / (1024.0 * 1024.0);   // kB → GB
        if (total_kB > 0)
            ram = 100.0 * (total_kB - avail_kB) / total_kB;
        else
            ram = 0.0;

        // -------- Disk usage (root partition via statvfs) --------
        struct statvfs stat;
        if (statvfs("/", &stat) == 0) {
            unsigned long long total_bytes = 
                static_cast<unsigned long long>(stat.f_blocks) * stat.f_frsize;
            unsigned long long free_bytes  = 
                static_cast<unsigned long long>(stat.f_bfree)  * stat.f_frsize;
            unsigned long long used_bytes  = total_bytes - free_bytes;
            hard_volume = used_bytes / (1024.0 * 1024.0 * 1024.0); // bytes → GB
        } else {
            hard_volume = -1.0;   // error
        }
        hard_ops = 0;
        hard_throughput = 0.0;
    }


void Mem_Check::checkCritical(Sign sign, float value, std::string metric, std::string metric_name){
    if (sign == LESS && metrics_[metric] < value) {
        createWarning(metric_name + " is below " + std::to_string(value) + "%");
    } else if (sign == GREATER && metrics_[metric] > value) {
        createWarning(metric_name + " is above " + std::to_string(value) + "%");
    } else if (sign == EQUAL && metrics_[metric] == value) {
        createWarning(metric_name + " is equal to " + std::to_string(value) + "%");
    } else if (sign == LESS_EQUAL && metrics_[metric] <= value) {
        createWarning(metric_name + " is below or equal to " + std::to_string(value) + "%");
    } else if (sign == GREATER_EQUAL && metrics_[metric] >= value) {
        createWarning(metric_name + " is above or equal to " + std::to_string(value) + "%");
    }
}

void Mem_Check::createWarning(std::string warning){
    std::lock_guard<std::mutex> lock(warning_mutex_);
    warnings_.push_back(warning);
}

int Mem_Check::mainLoop(){
    while(isRunning_.load()) {
        if (isPaused_.load()) {
            std::this_thread::sleep_for(1s);
            continue;
        }
        get_mem_metrics();
        std::lock_guard<std::mutex> lock(crit_value_mutex_);
        std::lock_guard<std::mutex> lock2(crit_sign_mutex_);
        for (auto &crit_value : crit_values_) {
            float value = metrics_[crit_value.first];
            Sign sign = crit_signs_[crit_value.first];
            checkCritical(sign, value, crit_value.first, crit_value.first);
        }
        std::this_thread::sleep_for(timer_);
    }
    return 0;
}

Mem_Check::Mem_Check(){
    ram_total =0.0f;
    ram =0.0f;
    hard_volume =0.0f;
    hard_ops =0;
    hard_throughput =0.0f;
    metrics_ = {
        {"ram_total", 0.0f},
        {"ram", 0.0f},
        {"hard_volume", 0.0f},
        {"hard_ops", 0},
        {"hard_throughput", 0.0f}
    };
    warnings_ = {};
    crit_values_ = {};
    crit_signs_ = {};
    timer_ = std::chrono::seconds(3);
}
Mem_Check::~Mem_Check(){
    // TODO: destructor logic
}

void Mem_Check::execute(){
    isRunning_.store(true);
    mainLoop();
}
void Mem_Check::update(std::string JsonConfig){
    std::lock_guard<std::mutex> lock(crit_value_mutex_);
    std::lock_guard<std::mutex> lock2(crit_sign_mutex_);
    nlohmann::json config = nlohmann::json::parse(JsonConfig);
    for (auto &crit_value : crit_values_) {
        crit_value.second = config["crit_value"][crit_value.first].get<float>();
    }
    timer_ = std::chrono::seconds(config["timer"].get<int>());
    for (auto &crit_value : crit_values_) {
        crit_signs_[crit_value.first] = static_cast<Sign>(config["crit_sign"][crit_value.first].get<int>());
    }
}
std::vector<std::string> Mem_Check::getWarnings(){
    return warnings_;
}
void Mem_Check::getMetrics(std::map<std::string, float> &metrics){
    std::lock_guard<std::mutex> lock(metric_mutex_);
    metrics = metrics_;
}
}