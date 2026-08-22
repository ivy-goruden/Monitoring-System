#include "Core/core.hpp"
#include "Gui/gui.hpp"
#include "Controller/controller.hpp"

int main(int argc, char **argv){
    
    std::string ui_file = "maket.glade";
    
    auto app = Gtk::Application::create("org.gtkmm.example");
    app->signal_activate().connect([app, ui_file]() {
        s21::Gui& gui = s21::Gui::getGui(ui_file);
        s21::Controller *controller = new s21::Controller(&gui);
        Core& core = Core::getCore();
        controller->addListner(s21::Subscription::onLogUpdate, &gui);
        controller->addListner(s21::Subscription::onAgentLoaded, &gui);
        controller->addListner(s21::Subscription::onAgentListUpdate, &gui);
        controller->addListner(s21::Subscription::onAgentUpdated, &Core::getCore());
        gui.run();
        auto window_ = gui.getWindow();
        app->add_window(*window_);
        window_->set_visible(true);
        window_->present();
        std::thread(core.mainLoop).detach();
    });
    return app->run(argc, argv);
}