#include "controller.hpp"
Controller::Controller(){
    logUpdated_ = false;
    core_ = Core::getCore();
    core_->addListner(Subscription::onLogUpdate, this);
    core_->addListner(Subscription::onMetricsUpdate, this);
}
Controller::~Controller(){
    delete core_;
}
void Controller::notify(const std::string event){
    if (event == Subscription::onLogUpdate){
        std::string logFileName = getTodayDate() + ".log";
        std::ofstream log(LOG_PATH_ + "/" + logFileName, std::ios::app);
        log << logMessage << std::endl;
        log.close();
    }
    if (event == Subscription::onMetricsUpdate){
        logUpdated_ = true;
    }
}
