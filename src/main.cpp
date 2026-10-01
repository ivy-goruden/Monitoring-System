#include "Controller/controller.hpp"
#include "Core/core.hpp"
#include "Gui/gui.hpp"
#include <locale>

int main(int argc, char **argv) {

  std::string ui_file = "maket.glade";

  std::setlocale(LC_ALL, "");
  auto app = Gtk::Application::create("org.gtkmm.example");
  s21::Gui &gui = s21::Gui::getGui(ui_file);
  auto controller = std::make_shared<s21::Controller>(&gui);
  app->signal_activate().connect([app, &gui, controller]() {
    Core &core = Core::getCore();
    controller->addListner(s21::Subscription::onLogUpdate, &gui);
    controller->addListner(s21::Subscription::onAgentLoaded, &gui);
    controller->addListner(s21::Subscription::onAgentListUpdate, &gui);
    controller->addListner(s21::Subscription::onAgentUpdated, &core);
    gui.addListner("EmailToggled", &core);
    gui.addListner("TgToggled", &core);
    std::thread(core.mainLoop).detach();
    gui.run();
    auto window_ = gui.getWindow();
    app->add_window(*window_);
    window_->set_visible(true);
    window_->present();
  });
  return app->run(argc, argv);
}