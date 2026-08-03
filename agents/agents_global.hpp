#ifndef AGENTS_GLOBAL_HPP
#define AGENTS_GLOBAL_HPP
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <map>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>
using namespace std::chrono_literals;
namespace s21{

    typedef enum Sign{
        LESS,
        GREATER,
        EQUAL,
        LESS_EQUAL,
        GREATER_EQUAL
    } Sign;

    class Agent{
        public:
            virtual ~Agent() = default;
            virtual void execute() = 0;
            virtual void update(std::string JsonConfig) = 0;
            virtual std::vector<std::string> getWarnings() = 0;
            virtual void getMetrics(std::map<std::string, float> &metrics) = 0;
            void kill(){
                isRunning_.store(false);
            }
            void stop(){
                isPaused_.store(true);
            }
            void start(){
                isPaused_.store(false);
            }
        protected:
            std::atomic<bool> isRunning_ = true;
            std::atomic<bool> isPaused_ = false;
    };
    struct CritValue_t{
        float value;
        Sign sign;
    };
}
#endif
