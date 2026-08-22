#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include "../global.hpp"
#include "../include/subscription.hpp"
#include "../Core/core.hpp"
#include "../include/chrono_helpers.hpp"
#include "../Gui/gui.hpp"
#include "../include/config_parser.hpp"
namespace s21{
    class Controller: public Listner, public Subscription{
        public:
            Controller(Gui* gui);
            ~Controller();
            void onNotify(const std::string event, json jsonData = json());
            // const Agent_t& getAgentInfo(AgentFile file);
            // void changeAgentName(AgentFile file, std::string name);
            // void changeAgentType(AgentFile file, std::string type);
            // void changeAgentCritVal(AgentFile file, std::string metric, float critVal, Sign sign);
            // void changeAgentUpdateTime(AgentFile file, std::string metric, Duration duration);
            // void changeAgentState(AgentFile file, bool active);
        private:
            std::string LOG_PATH_ = "../logs";
            Gui* gui_;
    };
}
#endif