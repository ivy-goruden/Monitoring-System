#include "subscription.hpp"

namespace s21{
    void Subscription::addListner(const std::string event, Listner* listener){
        removeListner(event, listener);
        std::lock_guard<std::mutex> lock(listeners_mutex_);
        listeners_.insert({event, listener});
    }
    void Subscription::removeListner(const std::string event, Listner* listener){
        std::lock_guard<std::mutex> lock(listeners_mutex_);
        for (auto it = listeners_.begin(); it != listeners_.end(); ++it){
            if (it->second == listener && it->first == event){
                listeners_.erase(it);
                break;
            }
        }
    }
    void Subscription::notify(const std::string event, json jsonData){
        std::vector<std::pair<const std::string, Listner*>> listeners;
        {
            std::lock_guard<std::mutex> lock(listeners_mutex_);
            for (auto& it : listeners_){
                if (it.first == event){
                    listeners.push_back(it);
                }
            }
        }

        for (const auto& [listener_event, listener] : listeners){
            if (listener == nullptr) {
                continue;
            }
            if (jsonData.empty()){
                listener->onNotify(listener_event);
            } else {
                listener->onNotify(listener_event, jsonData);
            }
        }
    }

}
