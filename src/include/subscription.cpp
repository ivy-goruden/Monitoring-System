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
        std::lock_guard<std::mutex> lock(listeners_mutex_);
        for (auto& it : listeners_){
            if (it.first == event){
                if (it.second != nullptr){
                    if (jsonData.empty()){
                        it.second->onNotify(event);
                    } else {
                        it.second->onNotify(event, jsonData);
                    }
                }
            }
        }
    }

}
