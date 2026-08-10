#include "Core/core.hpp"
#include "Gui/gui.hpp"
#include "Gui/app.hpp"

int main(int argc, char **argv){
    
    std::string ui_file = "maket.glade";
    Core& core = Core::getCore();
    std::thread(core.mainLoop).detach();
    auto app = Gtk::Application::create("org.gtkmm.example");
    app->signal_activate().connect([app, ui_file]() {
        s21::Gui& gui = s21::Gui::getGui(ui_file);
        gui.run();
        auto window_ = gui.getWindow();
        app->add_window(*window_);
        window_->set_visible(true);
        window_->present();
    });
    return app->run(argc, argv);
}