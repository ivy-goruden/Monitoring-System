#ifndef MEM_CHECK_HPP
#define MEM_CHECK_HPP
#include "agents_global.hpp"
namespace s21{
    class Mem_Check : public Agent
    {
    public:
        Mem_Check();
        ~Mem_Check();
        void execute() override;
        void update(std::string JsonConfig) override;
        std::vector<std::string> getWarnings() override;
        void getMetrics(std::map<std::string, float> &metrics) override;
    private:
        mutable std::mutex metric_mutex_;
        mutable std::mutex warning_mutex_; 
        std::map<std::string, float> metrics_;
        std::chrono::seconds timer_;
        std::vector<std::string> warnings_;
        mutable std::mutex crit_value_mutex_;
        std::map<std::string, float> crit_values_;
        mutable std::mutex crit_sign_mutex_;
        std::map<std::string, Sign> crit_signs_;

        void get_mem_metrics();
        void checkCritical(Sign sign, float value, std::string metric, std::string metric_name);
        void createWarning(std::string warning);
        int mainLoop();

        double ram_total;       // total RAM in GB
        double ram;        // current RAM usage in percent
        double hard_volume;      // used disk space in GB (for root partition)
        int    hard_ops;        // I/O operations per second (requires delta)
        double hard_throughput; // throughput in MB/s   (requires delta)

    };
}
#endif // CPU_CHECK_HPP