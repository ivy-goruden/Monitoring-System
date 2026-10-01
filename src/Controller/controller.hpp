#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#if __has_include("build_config.hpp")
#include "build_config.hpp"
#else
#define MONITORING_LOG_DIR "../logs"
#endif
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
        private:
            std::string LOG_PATH_ = MONITORING_LOG_DIR;
            Gui* gui_;
    };
}
#endif