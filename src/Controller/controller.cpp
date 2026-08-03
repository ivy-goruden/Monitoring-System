#include "controller.hpp"
Controller::Controller(){
    logUpdated_ = false;
    core_ = Core::getCore();
}
Controller::~Controller(){
    delete core_;
}
