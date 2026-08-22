#ifndef SUBSCRIPTION_HPP
#define SUBSCRIPTION_HPP
#include <map>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace s21{
    class Listner{
        public:
            Listner() = default;
            ~Listner() = default;
            virtual void onNotify(const std::string event, json jsonData = json()) = 0;
    };

    class Subscription{

        public:
            Subscription() = default;
            ~Subscription() = default;
            void addListner(const std::string event, Listner* listener);
            void removeListner(const std::string event, Listner* listener);
            void notify(const std::string event, json jsonData = json());

            static constexpr const char* onAgentLoaded = "onAgentLoaded"; //controller sent agent data
            static constexpr const char* onActiveAgentChange = "onActiveAgentChange"; //Gui requests a new agent
            static constexpr const char* onAgentUpdated = "onAgentUpdated"; //Gui updates the agent info
            static constexpr const char* onLogUpdate = "onLogUpdate";
            static constexpr const char* onMetricsUpdate = "onMetricsUpdate";
            static constexpr const char* onAgentListUpdate = "onAgentListUpdate";


        private:
            std::mutex listeners_mutex_;
            std::multimap <const std::string, Listner*> listeners_;
    };
}

#endif