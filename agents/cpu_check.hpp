#ifndef CPU_CHECK_HPP
#define CPU_CHECK_HPP
#include "agents_global.hpp"
namespace s21{
    class Cpu_Check : public Agent
    {
    public:
        Cpu_Check();
        ~Cpu_Check();
        void execute() override;
        void update(std::string JsonConfig) override;
        std::vector<std::string> getWarnings() override;
        void getMetrics(std::map<std::string, float> &metrics) override;
    private:
        mutable std::mutex mutex_;
        mutable std::mutex warning_mutex_; 
        std::map<std::string, float> metrics_;
        std::chrono::seconds timer_;
        std::vector<std::string> warnings_;
        std::atomic<float> crit_value_;
        std::atomic<Sign> crit_sign_;

        void get_cpu_times(size_t &idle_time, size_t &total_time);
        void checkCritical(Sign sign, float value, std::string metric);
        void createWarning(std::string warning);
        int mainLoop();

    };
}
#endif // CPU_CHECK_HPP
