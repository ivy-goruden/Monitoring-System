#include "gui.hpp"
#include <iostream>
namespace s21{
    Gui::Gui(std::string ui_file){
        ui_file_ = ui_file;
        window_ = nullptr;
        openButton_ = nullptr;
    }
    Gui::~Gui(){
    }

    void Gui::run(int argc, char** argv) {
        printf("run\n");
        Glib::RefPtr<Gtk::Builder> builder; //= Gtk::Builder::create_from_file(ui_file_);
        initObjects(builder);
        //AddSignal();
        window_->present();
        printf("run end\n");
    }
    Gui& Gui::getGui(std::string ui_file){;
        static Gui gui_(ui_file);
        return gui_;
    }

    void Gui::onActivate(int argc, char** argv, std::string ui_file){
        printf("onActivate\n");
        Gui& gui = Gui::getGui(ui_file);
        gui.run(argc, argv);
    }
    void Gui::initObjects(Glib::RefPtr<Gtk::Builder> builder){
        window_ = new Gtk::Window();
        window_->set_title("Monitoring System");
        window_->set_default_size(1000, 1000);
        auto button = Gtk::make_managed<Gtk::Button>("Click Me");
        button->signal_clicked().connect([]() {
            std::cout << "Button was clicked!" << std::endl;
        });

        // Add the button to the window
        window_->set_child(*button);
    }
    void Gui::AddSignal(){
        g_signal_connect(openButton_, "clicked", G_CALLBACK(onOpenButtonClick), nullptr);
    }
    void Gui::onOpenButtonClick(){
        std::cout << "Open Button clicked" << std::endl;
    }
}
