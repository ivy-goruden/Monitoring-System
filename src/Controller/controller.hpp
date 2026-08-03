#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include "../global.hpp"
namespace s21{
    class Controller{
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
            bool logUpdated_;
    };
}
#endif