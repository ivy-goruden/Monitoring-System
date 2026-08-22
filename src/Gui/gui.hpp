#ifndef GUI_HPP
#define GUI_HPP
#include "../global.hpp"
#include <gtkmm.h>
#include <map>
#include "../include/subscription.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace s21{
    struct Agent_Gui{
        std::string name;
        std::string type;
        std::string started_at;
        AgentFile file;
        std::map<std::string, std::string> crit_values;
        bool active;

    };

    struct ActiveAgent{
        Gtk::Entry* name;
        Gtk::Entry* type;

    };
    class Gui : public Listner, public Subscription{
        public:
            Gui(const Gui&) = delete;
            Gui& operator=(const Gui&) = delete;
            static Gui& getGui(std::string ui_file = "");
            void run();
            Gtk::Window* getWindow();
            std::string logMessage = "";
            std::mutex agentsMutex_;
            void setAgents(std::vector<Agent_Gui> agents);

            AgentFile getActiveAgent();
        private:
            AgentFile activeAgentFile_;
            Gtk::Window* window_;
            Glib::RefPtr<Gtk::TextBuffer> logBuffer_;
            std::string ui_file_;
            Gtk::ListBox* agentsList_;
            std::vector<Agent_Gui> agents_;
            Gtk::Box* info_list_;
            ActiveAgent *active_agent;
        private:
            Gui(std::string ui_file);
            ~Gui();
            void initObjects(Glib::RefPtr<Gtk::Builder> builder);
            void AddSignal();
            void onNotify(const std::string event, json jsonData = json());
            void updateAgentsList();
            Gtk::Widget* createAgentRow(const Agent_Gui* agent);
            void updateInfoList(Gtk::ListBoxRow* row);
            
    };

    inline void to_json(json& j, const Agent_Gui& agent) {
        j = json::object();
        j["name"] = agent.name;
        j["type"] = agent.type;
        j["active"] = agent.active;
        j["file"] = agent.file;

        json cv_map = json::object();
        for (auto const& [key, val] : agent.crit_values) {
            cv_map[key] = val;
        }
        j["crit_values"] = cv_map;

        j["started_at"] = agent.started_at;
    }

    inline void from_json(const json& j, Agent_Gui& agent) {
        agent.name = j["name"].get<std::string>();
        agent.type = j["type"].get<std::string>();
        agent.active = j["active"].get<bool>();
        agent.file = j["file"].get<std::string>();

        json cv_map = j["crit_values"];
        for (auto it = cv_map.begin(); it != cv_map.end(); ++it) {
            agent.crit_values[it.key()] = it.value().get<std::string>();
        }

        agent.started_at = j["started_at"].get<std::string>();
    }

    inline void to_json(json& j, const std::vector<Agent_Gui>& agents) {
        j = json::array();
        for (const auto& agent : agents) {
            json agent_json;
            to_json(agent_json, agent);
            j.push_back(agent_json);
        }
    }
    inline void from_json(const json& j, std::vector<Agent_Gui>& agents) {
        agents.clear();
        for (auto& it: j) {
            Agent_Gui agent;
            from_json(it, agent);
            agents.push_back(agent);
        }
    }
}
#endif