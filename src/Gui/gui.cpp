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
        Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file(ui_file_);
        initObjects(builder);
        AddSignal();
        window_->present();
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
        window_ = builder->get_widget<Gtk::Window>("main");
        window_->set_title("Monitoring System");
        window_->set_default_size(1000, 1000);
        openButton_ = builder->get_widget<Gtk::Button>("on_btn");
    }
    void Gui::AddSignal(){
        openButton_->signal_clicked().connect(
            sigc::mem_fun(*this, &Gui::onOpenButtonClick)
        );
    }
    void Gui::onOpenButtonClick(){
        std::cout << "Open Button clicked" << std::endl;
    }
}
