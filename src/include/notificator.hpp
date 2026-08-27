#ifndef NOTIFICATOR_HPP
#define NOTIFICATOR_HPP
#include <string>
#include <chrono>
using namespace std::chrono_literals;
namespace s21{
    class Notificator{
        public:
            Notificator();
            ~Notificator() = default;
            int sendEmail(std::string subject, std::string body);
            int sendTelegram(std::string message);
    
        private:
            std::string password;
            std::string token;
            std::string gmail;
            std::string chat_id;
            std::string authToken;
            std::string receiver;
            std::chrono::steady_clock::time_point last_sent;
            std::chrono::steady_clock::duration cooldown;
        };
}
#endif
