#ifndef GUI_HPP
#define GUI_HPP
#include "../global.hpp"
#include <gtkmm.h> 
//#include "../Controller/controller.hpp"
namespace s21{
    class Gui : public Gtk::Application{
        public:
            Gui(const Gui&) = delete;
            Gui& operator=(const Gui&) = delete;
            static Gui& getGui(std::string ui_file = "");
            void run(int argc, char** argv);
            static void onActivate(int argc, char** argv, std::string ui_file);
        private:
            std::map<AgentFile, bool> agents_;
            size_t activeAgent_;
            //Controller c_;
            Gtk::Window* window_;
            Gtk::Button* openButton_;
            std::string ui_file_;
        private:
            Gui(std::string ui_file);
            ~Gui();
            void initObjects(Glib::RefPtr<Gtk::Builder> builder);
            void AddSignal();
            void onOpenButtonClick();

            
    };
}
#endif