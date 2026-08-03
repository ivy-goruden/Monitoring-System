#include "config_parser.hpp"
#include <toml++/toml.hpp>
#include <iostream>



namespace s21{
    ConfigParser::ConfigParser(){}
    ConfigParser::~ConfigParser(){}

    Agent_t ConfigParser::parseConfig(ConfFile file){
        Agent_t agent;
        std::ofstream config_file(file, std::ios::out);
        if (!config_file.is_open()){
            std::cerr << "Error: Failed to open config file.\n";
            config_file.close();
            return agent;
        }
        config_file.close();
        auto tbl = toml::parse_file(file);

        std::string name = tbl["agent"]["name"].value_or(default_agent_name);
        std::string type = tbl["agent"]["type"].value_or(default_agent_type);
        std::map<std::string, CritValue_t> crit_values;
        std::map<std::string, Duration> update_times;
        if (auto values = tbl["crit_values"].as_array()) {
            CritValue_t crit_value;
            for (auto&& node : *values) {
                // Cast the node to a table
                if (auto* value = node.as_table()) {
                    std::string name = (*value)["name"].value_or("");
                    std::string value_str = (*value)["value"].value_or("=" + std::to_string(default_crit_value));

                    std::string sign_str = value_str.substr(0, 1);
                    crit_value.sign = parseSign(sign_str);
                    crit_value.value = std::stof(value_str.substr(1));
                    crit_values[name] = crit_value;
                    
                }
            }
        }
        if (auto timer = tbl["update_time"].as_array()) {
            for (auto&& node : *timer) {
                if (auto* time = node.as_table()) {
                    std::string name = (*time)["name"].value_or("");
                    update_times[name] = Duration(std::stoi((*time)["value"].value_or(std::to_string(default_update_time).c_str())));
                }
            }
        }
            
        agent.name = name;
        agent.type = type;
        agent.crit_values = crit_values;
        agent.update_time = update_times;
        agent.started_at = std::chrono::system_clock::now();
        agent.handle = nullptr;
        return agent;

    }

    int ConfigParser::saveConfig(Agent_t &agent, ConfFile conf){
        toml::table tbl;
        tbl.insert_or_assign("agent", toml::table{});
        auto agent_sub = tbl["agent"].as_table();
        (*agent_sub).insert_or_assign("name", agent.name);
        (*agent_sub).insert_or_assign("type", agent.type);
        (*agent_sub).insert_or_assign("file", agent.file);
        (*agent_sub).insert_or_assign("active", agent.active);
        (*agent_sub).insert_or_assign("started_at", agent.started_at.time_since_epoch().count());
        
        tbl.insert_or_assign("crit_values", toml::array{});
        auto crit_values = tbl["crit_values"].as_array();
        for (auto& [name, value] : agent.crit_values) {
            toml::table critTab;
            critTab.insert_or_assign("name", name);
            critTab.insert_or_assign("value", to_string(value.sign) + std::to_string(value.value));
            crit_values->push_back(critTab);
        }
        tbl.insert_or_assign("update_time", toml::array{});
        auto update_time = tbl["update_time"].as_array();
        for (auto& [name, time] : agent.update_time) {
            toml::table updateTab;
            updateTab.insert_or_assign("name", name);
            updateTab.insert_or_assign("value", time.count());
            update_time->push_back(updateTab);
        }

        // 4. Write to the configuration file
        std::ofstream file(conf);   // adjust to your actual ConfFile member
        if (!file.is_open()) {
            return -1;   // error
        }
        file << tbl;
        file.close();

         return 0;
    }
}
