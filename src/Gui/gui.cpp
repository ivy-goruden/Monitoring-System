#include "gui.hpp"
#include <iostream>
namespace s21{
    Gui::Gui(std::string ui_file){
        ui_file_ = ui_file;
        window_ = nullptr;
        logBuffer_ = nullptr;
        agentsList_ = nullptr;
        info_list_ = nullptr;
        active_agent = nullptr;
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
        logBuffer_ = builder->get_object<Gtk::TextBuffer>("log");
        if (logBuffer_ == nullptr){
            logBuffer_ = Gtk::TextBuffer::create();
        }

        agentsList_ = builder->get_widget<Gtk::ListBox>("agents_list");
        info_list_ = builder->get_widget<Gtk::Box>("info_list");
        window_ = builder->get_widget<Gtk::Window>("main");
        window_->set_default_size(800, 600);
        window_->set_title("Monitoring System");
    }
    void Gui::AddSignal(){
        agentsList_->signal_row_selected().connect(
            sigc::mem_fun(*this, &Gui::updateInfoList)
        );
    }

    Gtk::Window* Gui::getWindow(){
        return window_;
    }

    AgentFile Gui::getActiveAgent(){
        return activeAgentFile_;
    }

    void Gui::onNotify(std::string event, json jsonData){
        if (event == Subscription::onLogUpdate){
            Glib::signal_idle().connect([this]() -> bool {
                if (logBuffer_ == nullptr) return false;
                logBuffer_->set_text(logMessage);
                return false; // выполнить только один раз
            });
        }
        if (event == Subscription::onAgentListUpdate){
            Glib::signal_idle().connect([this, jsonData]() -> bool {
                std::vector<Agent_Gui> agents = jsonData.get<std::vector<Agent_Gui>>();
                setAgents(agents);
                updateAgentsList();
                return false; // выполнить только один раз
            });
        }
    }

    Gtk::Widget* Gui::createAgentRow(const Agent_Gui* agent){
        auto row = Gtk::manage(new Gtk::ListBoxRow());
        row->set_name(agent->file); // Устанавливаем имя строки как имя файла агента
        auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL, 10));
        
        auto name_label = Gtk::manage(new Gtk::Label(agent->name));
        auto active_switch = Gtk::manage(new Gtk::Switch());
        active_switch->set_active(agent->active);
        
        box->append(*name_label);
        box->append(*active_switch);
        
        row->set_child(*box);
        
        return row;
    }

    void Gui::updateAgentsList(){
        if (agentsList_ == nullptr) return;
        for (auto& child : agentsList_->get_children()) {
            agentsList_->remove(*child);
        }
        for (const auto& agent : agents_) {
            Gtk::Widget* row = createAgentRow(&agent);
            agentsList_->append(*row);
        }
    }

    void Gui::setAgents(std::vector<Agent_Gui> agents){
        std::lock_guard<std::mutex> lock(agentsMutex_);
        agents_ = agents;
    }

    void Gui::updateInfoList(Gtk::ListBoxRow* row) {
        int index = row->get_index();
        if (index < 0 || index >= static_cast<int>(agents_.size())) return;
        Agent_Gui &activeAgent = agents_[index];
        if (info_list_ == nullptr) return;
        for (auto& child : info_list_->get_children()) {
            info_list_->remove(*child);
        }

        
        auto name = Gtk::manage(new Gtk::Label("Name: "));
        info_list_->append(*name);
        Gtk::Entry* name_value = Gtk::manage(new Gtk::Entry());
        name_value->set_editable(true);
        name_value->set_can_focus(true);
        name_value->set_focus_on_click(true); 
        name_value->get_buffer()->set_text(activeAgent.name);
        info_list_->append(*name_value);
        auto type = Gtk::manage(new Gtk::Label("Type: "));
        info_list_->append(*type);
        Gtk::Entry* type_value = Gtk::manage(new Gtk::Entry());
        type_value->set_editable(true);
        type_value->set_can_focus(true);
        type_value->get_buffer()->set_text(activeAgent.type);
        info_list_->append(*type_value);
        auto started_at = Gtk::manage(new Gtk::Label("Started At: " + std::string(activeAgent.started_at)));
        info_list_->append(*started_at);
        auto metrics = Gtk::manage(new Gtk::Label("Critical Values: "));
        info_list_->append(*metrics);
        for (const auto& [metric, value] : activeAgent.crit_values) {
            auto metric_label = Gtk::manage(new Gtk::Label(metric + ": " + value));
            info_list_->append(*metric_label);
        }
        ActiveAgent* a_agent = new ActiveAgent();
        a_agent->name = name_value;
        a_agent->type = type_value;
        active_agent = a_agent;
        auto update_button = Gtk::manage(new Gtk::Button("Update"));
         update_button->signal_clicked().connect([this, index]() {
            Agent_Gui &activeAgent = agents_[index];
            std::string new_name = this->active_agent->name->get_buffer()->get_text();
            std::string new_type = this->active_agent->type->get_buffer()->get_text();
            activeAgent.name = new_name;
            activeAgent.type = new_type;
            json j = activeAgent;
            notify(Subscription::onAgentUpdated, j);
        });
        info_list_->append(*update_button);
    }

}
