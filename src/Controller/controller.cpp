#include "controller.hpp"
using namespace s21;
Controller::Controller(Gui* gui){
    Core &core = Core::getCore();
    core.addListner(Subscription::onLogUpdate, this);
    core.addListner(Subscription::onMetricsUpdate, this);
    core.addListner(Subscription::onAgentListUpdate, this);
    //gui->addListner(Subscription::onActiveAgentChange, this);
    gui->addListner(Subscription::onAgentUpdated, this);
    gui_ = gui;
}
Controller::~Controller(){
}
void Controller::onNotify(const std::string event, json jsonData){
    if (event == Subscription::onLogUpdate){
        std::string logFileName = getTodayDate() + ".log";
        std::ifstream log(LOG_PATH_ + "/" + logFileName, std::ios::in);
        std::string logMessage;
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(log, line)) {
            lines.push_back(line);
        }
        if (lines.empty()){
            log.close();
            return;

        }
        std::vector<std::string> last_lines(lines.end() - std::min(int(lines.size()),10), lines.end());
        for (const auto& l : last_lines) {
            logMessage += l + "\n";
        }
        gui_->logMessage = logMessage;
        notify(Subscription::onLogUpdate);
        log.close();
    }
    if (event == Subscription::onMetricsUpdate){
    }
    if (event == Subscription::onAgentListUpdate){
        Core &core = Core::getCore();
        auto agents = core.getAgents();
        std::vector<Agent_Gui> agentMap;
        for (auto& agent : agents) {
            Agent_Gui agentGui;
            agentGui.name = agent.name;
            agentGui.type = agent.type;
            agentGui.active = agent.active;
            agentGui.file = agent.file;
            agentGui.started_at = agent.started_at;
            for (auto& [metric, critValue] : agent.crit_values) {
                std::string signStr = to_string(critValue.sign);
                agentGui.crit_values[metric] = signStr + std::to_string(critValue.value);
            }
            agentMap.push_back(agentGui);
        }
        json j = agentMap;
        notify(Subscription::onAgentListUpdate, j);
    }
    if (event == Subscription::onActiveAgentChange){
        Core &core = Core::getCore();
        Gui &gui = Gui::getGui();
        AgentFile activeAgent = gui.getActiveAgent();
        auto agents = core.getAgents();

    }

    if (event == Subscription::onAgentUpdated){
        Agent_t agent = Agent_t();
        Agent_Gui agentGui = jsonData.get<Agent_Gui>();
        agent.name = agentGui.name;
        agent.type = agentGui.type;
        agent.active = agentGui.active;
        agent.file = agentGui.file;
        agent.started_at = agentGui.started_at;
        for (auto& [metric, critValueStr] : agentGui.crit_values) {
            Sign sign = parseSign(critValueStr.substr(0, 1));
            float value = std::stof(critValueStr.substr(1));
            CritValue_t critValue{value, sign};
            agent.crit_values[metric] = critValue;
        }
        ConfFile confFile = Core::getCore().getConfFile(agent.file);
        ConfigParser::saveConfig(agent, confFile);
        json agent_file = agent.file;
        notify(Subscription::onAgentUpdated, agent_file);
    }

    
}


