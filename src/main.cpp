#include "Core/core.hpp"
#include "Gui/gui.hpp"

int main(int argc, char **argv){
    
    std::string ui_file = "maket.glade";
    Core& core = Core::getCore();
    std::thread(core.mainLoop).detach();
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.example.myapp");
    app->signal_activate().connect([argc, argv, ui_file]() {
        Gui::onActivate(argc, argv, ui_file);
    });
    return app->run(argc, argv);
}