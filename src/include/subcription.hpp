#ifndef SUBSCRIPTION_HPP
#define SUBSCRIPTION_HPP
#include <map>
#include <string>
namespace s21{

    class Subscription{
        public:
            Subscription() = default;
            ~Subscription() = default;
            void addListner(const std::string event, Listner* listener);
            void removeListner(const std::string event, Listner* listener);
            void notify(const std::string event);

            static const std::string onAgentChange = "onAgentChange";
            static const std::string onLogUpdate = "onLogUpdate";
            static const std::string onMetricsUpdate = "onMetricsUpdate";


        private:
            std::mutex listeners_mutex_;
            std::multimap <const std::string, Listner*> listeners_;
    }

    class Listner{
        public:
            Listner() = default;
            ~Listner() = default;
            virtual void notify(const std::string event) = 0;
    }
}

#endif