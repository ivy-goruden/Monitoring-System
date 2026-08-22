#ifndef CORE_HPP
#define CORE_HPP
#include "../global.hpp"
#include <regex>
#include <algorithm>
#include <filesystem> 
namespace fs = std::filesystem;
#include <dlfcn.h>
#include <future>
#include <mutex>
#include <memory>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <iostream>
#include <type_traits>
#include <fstream>
#include "../include/chrono_helpers.hpp"
#include "../include/subscription.hpp"
using namespace s21;
class Core: public Subscription, public Listner{
    public:
        std::vector<Agent_t> getAgents();
        const Agent_t* getAgent(AgentFile file);
        Duration timer_;
        std::atomic<bool> isRunning_;
        Core(const Core&) = delete;
        Core& operator=(const Core&) = delete;
        static Core& getCore();
        static void mainLoop();
        void onNotify(const std::string event, json jsonData);
        ConfFile getConfFile(AgentFile file);
    private:
        mutable std::shared_mutex agentMutex_;
        mutable std::mutex metricsMutex_;
        mutable std::mutex outdatedAgentsMutex_;
        std::vector<Agent_t> agents_;
        std::map<std::string, float> metrics_; //data we will log to file
        std::vector<AgentFile> outdatedAgents_; //агенты, которые устарели
        Date today_;
        const std::string AGENT_PATH = "../agents/";
        const std::string LOG_PATH = "../logs/";

        int validateAgentFile(AgentFile file);
        std::map<std::string, CritValue_t> getCriticalValues(AgentFile file);
        std::map<std::string, Duration> getUpdateTime(AgentFile file);
        void AddAgent(AgentFile file);
        void RemoveAgent(AgentFile file);
        std::vector<AgentFile> getNewAgents(); //новые агенты в папке
        std::vector<AgentFile> getMissingAgents(); //агенты, удаленые из папки
        std::string getLogFileName();

        void* runAgent(Agent_t& agent);
        void startAgent(Agent_t& agent);
        void stopAgent(Agent_t& agent);
        void killAgent(Agent_t& agent);
        void updateAgent(Agent_t& updated_agent);
        int checkIfFuncExists(void* handle, const std::string& funcName);
        Core();
        ~Core();
        void stop();
        std::map<std::string, float> getMetrics(Agent_t& agent);
        //async
        std::future<void> mainLoopFuture_;
        static void WriteLogs();
        static void UpdateMetrics();
        static void CheckAgents();
        Agent_t* getAgentMod(AgentFile file);
};
#endif