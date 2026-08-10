#include "gui.hpp"
#include <iostream>
namespace s21{
    Gui::Gui(std::string ui_file){
        ui_file_ = ui_file;
        openButton_ = nullptr;
        window_ = nullptr;
    }
    Gui::~Gui(){
    }

    void Gui::run() {
        printf("run\n");
        Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file(ui_file_);
        initObjects(builder);
        AddSignal();
    }
    Gui& Gui::getGui(std::string ui_file){;
        static Gui gui_(ui_file);
        return gui_;
    }

    void Gui::initObjects(Glib::RefPtr<Gtk::Builder> builder){
        openButton_ = builder->get_widget<Gtk::Button>("on_btn");
        window_ = builder->get_widget<Gtk::Window>("main");
        window_->set_default_size(800, 600);
        window_->set_title("Monitoring System");
    }
    void Gui::AddSignal(){
        openButton_->signal_clicked().connect(
            sigc::mem_fun(*this, &Gui::onOpenButtonClick)
        );
    }
    void Gui::onOpenButtonClick(){
        std::cout << "Open Button clicked" << std::endl;
    }

    Gtk::Window* Gui::getWindow(){
        return window_;
    }
}
