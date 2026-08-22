#ifndef GLOBAL_HPP
#define GLOBAL_HPP
#include <map>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace s21{
    typedef enum Sign{
        LESS,
        GREATER,
        EQUAL,
        LESS_EQUAL,
        GREATER_EQUAL
    } Sign;
    struct CritValue_t{
        float value;
        Sign sign;
    };
    typedef std::chrono::system_clock::time_point TimePoint;
    typedef std::chrono::year_month_day Date;
    typedef std::chrono::seconds Duration;
    typedef std::string AgentFile; // имя файла агента
    typedef std::string ConfFile; // имя файла конфига
    struct Agent_t{
        bool active;
        std::map<std::string, CritValue_t> crit_values;
        std::map<std::string, Duration> update_time;
        std::string started_at;
        std::string name;
        std::string type;
        AgentFile file;
        void* handle;
        bool operator==(const Agent_t& other) const {
            return file == other.file;
        }
    };

    const std::string default_agent_name = "agent";
    const std::string default_agent_type = "type";
    const float default_crit_value = 0.0f;
    const Sign default_crit_sign = EQUAL;
    const int default_update_time = 1000; //seconds

    inline Sign parseSign(const std::string& sign_str) {
        if (sign_str == "<")        return LESS;
        if (sign_str == ">")        return GREATER;
        if (sign_str == "=")        return EQUAL;
        if (sign_str == "≤")        return LESS_EQUAL;
        if (sign_str == "≥")        return GREATER_EQUAL;
        return default_crit_sign;   // or throw an error

    }
    inline std::string to_string(Sign& s){
        switch (s) {
            case LESS:          return "<";
            case GREATER:       return ">";
            case EQUAL:         return "=";
            case LESS_EQUAL:    return "≤";
            case GREATER_EQUAL: return "≥";
            default:            return "=";
        }
    }
    inline void to_json(json& j, const Sign& s) {
        switch (s) {
            case LESS:          j = "<"; break;
            case GREATER:       j = ">"; break;
            case EQUAL:         j = "="; break;
            case LESS_EQUAL:    j = "≤"; break;
            case GREATER_EQUAL: j = "≥"; break;
        }
    }

    inline void to_json(json& j, const CritValue_t& cv) {
        j = json{{"value", cv.value}, {"sign", cv.sign}};
    }

    inline void to_json(json& j, const Duration& d) {
        j = d.count();
    }

    inline void to_json(json& j, const TimePoint& tp) {
        auto time_t = std::chrono::system_clock::to_time_t(tp);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        j = std::string(buf);
    }

    inline void to_json(json& j, const Agent_t& agent) {
        j = json::object();
        j["name"] = agent.name;
        j["type"] = agent.type;
        j["active"] = agent.active;
        
        json cv_map = json::object();
        for (auto const& [key, val] : agent.crit_values) {
            cv_map[key] = val;
        }
        j["crit_values"] = cv_map;

        json ut_map = json::object();
        for (auto const& [key, val] : agent.update_time) {
            ut_map[key] = val.count();
        }
        j["update_time"] = ut_map;

        j["started_at"] = agent.started_at;
    }
}
#endif