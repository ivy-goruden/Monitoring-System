#include "core.hpp"
#include "../include/config_parser.hpp"
using namespace s21;

static std::atomic<bool> writeLogsFlag(false);
static std::atomic<bool> updateMetricsFlag(false);
static std::atomic<bool> checkAgentsFlag(false);



Core::Core(){
    outdatedAgents_ = {};
    metrics_ = {};
    today_ = Date();
    timer_ = Duration(1);
    isRunning_.store(true);
}
Core::~Core(){
    stop();
}
Core& Core::getCore() {
    static Core core_; // Создается строго один раз при первом вызове
    return core_;     // Возвращаем ссылку, а не копию!
}

void Core::mainLoop(){
    printf("Main loop started\n");
    Core& core = getCore();
    while (core.isRunning_.load()){
        if (writeLogsFlag.load()){
            std::thread(core.WriteLogs).detach();
            writeLogsFlag.store(false);
        }
        if (updateMetricsFlag.load()){
            std::thread(core.UpdateMetrics).detach();
            updateMetricsFlag.store(false);
        }
        if (checkAgentsFlag.load()){
            std::thread(core.CheckAgents).detach();
            checkAgentsFlag.store(false);
        }
        std::this_thread::sleep_for(std::chrono::seconds(core.timer_));
    }
    printf("Main loop stopped\n");
}

void Core::stop(){
    isRunning_.store(false);
}



std::vector<AgentFile> Core::getNewAgents(){
    std::string pattern = ".so";
    if (!fs::exists(AGENT_PATH) || !fs::is_directory(AGENT_PATH)) {
        std::cerr << "Error: Agent directory does not exist.\n";
        return {};
    }
    std::vector<AgentFile> agents;
    std::vector<std::string> agents_files;
    std::shared_lock lock(agentMutex_);
    for (const auto& agent : agents_) {
        agents_files.push_back(agent.file);
    }
    for (const auto& entry : fs::directory_iterator(AGENT_PATH)) {
        if (fs::is_regular_file(entry.path()) && std::string(entry.path().filename()).ends_with(pattern)) {
            if (std::find(agents_files.begin(), agents_files.end(), entry.path().string()) == agents_files.end()) {
                agents.push_back(entry.path().string());
                printf("New agent: %s\n", entry.path().string().c_str());
            }
        }
    }
    return agents;

}
std::vector<AgentFile> Core::getMissingAgents(){
    std::string pattern = ".so";
    if (!fs::exists(AGENT_PATH) || !fs::is_directory(AGENT_PATH)) {
        std::cerr << "Error: Agent directory does not exist.\n";
        return {};
    }
    std::vector<AgentFile> found_agents;
    std::vector<AgentFile> missing_agents;
    for (const auto& entry : fs::directory_iterator(AGENT_PATH)) {
        if (fs::is_regular_file(entry.path()) && std::string(entry.path().filename()).ends_with(pattern)) {
                found_agents.push_back(entry.path().string());
        }
    }
    std::shared_lock lock(agentMutex_);
    for (const auto& agent : agents_) {
        if (std::find(found_agents.begin(), found_agents.end(), agent.file) == found_agents.end()) {
            missing_agents.push_back(agent.file);
            printf("Missing agent: %s\n", agent.file.c_str());
        }
    }
    return missing_agents;
}

int Core::validateAgentFile(AgentFile file){
    void* handle;
    handle = dlopen(file.c_str(), RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }
    std::vector<std::string> funcNames = {"execute", "getMetrics", "getWarnings", "stop", "start", "kill", "update"};
    for (const std::string &funcName : funcNames){
        if (checkIfFuncExists(handle, funcName) != 0){
            dlclose(handle);
            return 1;
        }
    }
    dlclose(handle);
    return 0;
}

int Core::checkIfFuncExists(void* handle, const std::string& funcName){
    void* func = dlsym(handle, funcName.c_str());
    if (func == nullptr) {
        fprintf(stderr, "Ошибка поиска функциа '%s': %s\n", funcName.c_str(), dlerror());
        return 1;
    }
    return 0;
}

ConfFile Core::getConfFile(AgentFile file){
    std::string pattern = file.substr(0, file.find_last_of("."))  + ".conf";
    std::ofstream config_file(pattern, std::ios::out);
    if (!config_file.is_open()){
        std::cerr << "Error: Failed to open config file.\n";
        config_file.close();
        return "";
    }
    config_file.close();
    return pattern;
}

void* Core::runAgent(Agent_t& agent){
    printf("Running agent %s\n", agent.file.c_str());
    void (*execute)();
    void* handle = dlopen(agent.file.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        std::cerr << "dlopen error: " << dlerror() << std::endl;
        return nullptr;
    }
    *(void **) (&execute) = dlsym(handle, "execute");
    std::thread(execute).detach();
    return handle;
}
void Core::stopAgent(Agent_t& agent){
    if (agent.handle == nullptr){
        printf("Agent %s: handle is null\n", agent.file.c_str());
        return;
    }
    void (*stop)();
    *(void **) (&stop) = dlsym(agent.handle, "stop");
    stop();// останавливаем главный цикл
}
void Core::killAgent(Agent_t& agent){
    void (*kill)();
    if (agent.handle == nullptr){
        return;
    }
    *(void **) (&kill) = dlsym(agent.handle, "kill");
    kill();// убиваем главный цикл
    dlclose(agent.handle);
}



void Core::AddAgent(AgentFile file){
    if (getAgentMod(file) !=nullptr){
        return;
    }
    std::unique_lock lock(agentMutex_);
    if (validateAgentFile(file) != 0){
        return;
    }
    ConfFile confFile = getConfFile(file);//проверяем существует ли конфиг
    printf("confFile: %s\n", confFile.c_str());

    if (confFile != ""){
        Agent_t agent = ConfigParser::parseConfig(confFile);
        agent.active = true;
        agent.file = file;
        agent.handle = runAgent(agent);
        agents_.push_back(agent);
        Agent_t& addedAgent = agents_.back();
        if (agent.active){
            startAgent(addedAgent);
        }else{
            stopAgent(addedAgent);
        }
        ConfigParser::saveConfig(agent, confFile);
    }else{
        printf("Agent %s: No config file found\n", file.c_str());
    }
}

void Core::RemoveAgent(AgentFile file){
    Agent_t* agent = getAgentMod(file);
    if (agent != nullptr){
        killAgent(*agent);
        std::shared_lock lock(agentMutex_);
        agents_.erase(std::remove(agents_.begin(), agents_.end(), *agent), agents_.end());
    }
}

void Core::startAgent(Agent_t& agent){
    if (agent.handle == nullptr){
        printf("Agent %s: handle is null\n", agent.file.c_str());
        return;
    }
    agent.active = true;
    void (*start)();
    *(void **) (&start) = dlsym(agent.handle, "start");
    start();// запускаем главный цикл
}

void Core::updateAgent(Agent_t& updated_agent){
    Agent_t* oldAgent = getAgentMod(updated_agent.file);
    if (oldAgent == nullptr){
        return;
    }
    *oldAgent = updated_agent;
    json j = updated_agent;
    std::string json_string = j.dump(4);
    void (*update)(std::string);
    *(void **) (&update) = dlsym(updated_agent.handle, "update");
    update(json_string);
}


const Agent_t* Core::getAgent(AgentFile file){
    std::shared_lock lock(agentMutex_);
    for (const auto& agent : agents_) {
        if (agent.file == file) {
            return &agent;
        }
    }
    return nullptr;
}
Agent_t* Core::getAgentMod(AgentFile file){
    std::shared_lock lock(agentMutex_);
    for (auto& agent : agents_) {
        if (agent.file == file) {
            return &agent;
        }
    }
    return nullptr;
}

void Core::CheckAgents(){
    printf("Checking agents...\n");
    Core& core = getCore();
    std::vector<AgentFile> missing_agents = core.getMissingAgents();
    for (const auto& agent : missing_agents) {
        core.RemoveAgent(agent);
    }
    std::vector<AgentFile> new_agents = core.getNewAgents();
    for (const auto& agent : new_agents) {
        core.AddAgent(agent);
    }
    std::lock_guard<std::mutex> lock(core.outdatedAgentsMutex_);
    for (const auto& agentFile : core.outdatedAgents_) {
        Agent_t updatedAgent = ConfigParser::parseConfig(core.getConfFile(agentFile));
        core.updateAgent(updatedAgent);
        Agent_t *newAgent = core.getAgentMod(agentFile);
        if (updatedAgent.active){
            core.startAgent(*newAgent);
        }else{
            core.stopAgent(*newAgent);
        }

    }
    checkAgentsFlag.store(true);
}

std::string Core::getLogFileName(){
    std::string todayDate = getTodayDate();
    return todayDate + ".log";
}

void Core::WriteLogs(){
    printf("Writing logs...\n");
    Core& core = getCore();
    //создаем файл для записи логов либо открываем старый
    std::string logFileName = core.getLogFileName();

    std::string logMessage = "";
    logMessage += getTodayDate() + "|";
    {
        std::lock_guard<std::mutex> lock(core.metricsMutex_);
        for (auto metric : core.metrics_) {
            logMessage += metric.first + ":" + std::to_string(metric.second) + "|";
        }
    }
    logMessage+='\n';
    std::ofstream log(core.LOG_PATH + "/" + logFileName, std::ios::app);
    log << logMessage << std::endl;
    log.close();
    printf("Logs written successfully\n");
    notify(onLogUpdate);
    writeLogsFlag.store(true);
}

std::map<std::string, float> Core::getMetrics(Agent_t& agent){
    std::map<std::string, float> metrics;
    void (*getMetrics)(std::map<std::string, float>&);
    *(void **) (&getMetrics) = dlsym(agent.handle, "getMetrics");
    getMetrics(metrics);
    return metrics;
}

void Core::UpdateMetrics(){
    printf("Updating metrics...\n");
    Core& core = getCore();

    std::vector<Agent_t> agents;
    std::map<std::string, float> globalMetrics;
    {
        std::shared_lock lock(core.agentMutex_);
        for (auto& agent : core.agents_) {
            if (agent.active){
                agents.push_back(agent);
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(core.metricsMutex_);
        globalMetrics = core.metrics_;
    }
    for (auto& agent : agents) {
        if (agent.active){
            std::map<std::string, float> metrics = core.getMetrics(agent);
            for (auto metric : metrics) {
                globalMetrics[metric.first] = metric.second;
            }
        }
    }
    std::lock_guard<std::mutex> lock2(core.metricsMutex_);
    for (auto metric : globalMetrics) {
        core.metrics_[metric.first] = metric.second;
    }
    std::cout << "Metrics updated successfully" << std::endl;
    updateMetricsFlag.store(true);
}


