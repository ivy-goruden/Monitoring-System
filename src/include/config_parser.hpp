#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP
#include "../global.hpp"
namespace s21{
    class ConfigParser{
        public:
            ConfigParser();
            ~ConfigParser();
            static Agent_t parseConfig(ConfFile file);
            static int saveConfig(Agent_t &agent, ConfFile confFile);
    };
}
#endif