#ifndef SWAP_CHECK_HPP
#define SWAP_CHECK_HPP

#include "agents_global.hpp"

namespace s21 {
class Swap_Check : public Agent {
public:
    Swap_Check();
    ~Swap_Check();

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
    std::atomic<float> crit_value_;
    std::atomic<Sign> crit_sign_;

    void get_swap_metrics();
    void checkCritical(Sign sign, float value, std::string metric_name);
    void createWarning(const std::string& warning);
    int mainLoop();
};
}

#endif // SWAP_CHECK_HPP
