#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include "../global.hpp"
#include "../include/subcription.hpp"
#include "../Core/core.hpp"
#include "../include/chrono_helpers.hpp"
namespace s21{
    class Controller: public Listner{
        public:
            Controller();
            ~Controller();
            const Agent_t& getAgentInfo(AgentFile file);
            void changeAgentName(AgentFile file, std::string name);
            void changeAgentType(AgentFile file, std::string type);
            void changeAgentCritVal(AgentFile file, std::string metric, float critVal, Sign sign);
            void changeAgentUpdateTime(AgentFile file, std::string metric, Duration duration);
            void changeAgentState(AgentFile file, bool active);
            void notifyLogUpdated();
            std::string getLog();
        private:
            Core core_;
            std::string LOG_PATH_ = "../logs";
    };
}
#endif