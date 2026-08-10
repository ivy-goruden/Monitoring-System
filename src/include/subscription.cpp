#include "subscription.hpp"

namespace s21{
    Subscription::Subscription() = default;
    Subscription::~Subscription() = default;
    void Subscription::addListner(const std::string event, Listner* listener){
        removeListner(event, listener);
        std::lock_guard<std::mutex> lock(listeners_mutex_);
        listeners_.insert({event, listener});
    }
    void Subscription::removeListner(const std::string event, Listner* listener){
        std::lock_guard<std::mutex> lock(listeners_mutex_);
        for (auto& it : listeners_){
            if (it.second == listener && it.first == event){
                listeners_.erase(it);
                break;
            }
        }
    }
    void Subscription::notify(const std::string event){
        std::lock_guard<std::mutex> lock(listeners_mutex_);
        for (auto& it : listeners_){
            if (it.first == event){
                if (it.second != nullptr){
                    it.second->notify(event);
                }
            }
        }
    }

    Listner::Listner() = default;
    Listner::~Listner() = default;

}
