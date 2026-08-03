#ifndef NOTIFICATOR_HPP
#define NOTIFICATOR_HPP
#include <string>
namespace s21{
    class Notificator{
        public:
            Notificator();
            ~Notificator() = default;


            int sendEmail(std::string subject, std::string body, std::string email);
            int sendTelegram(std::string message, std::string chat_id);
    
        private:
            std::string smtp_server_;
            int smtp_port_;
            std::string smtp_username_;
            std::string smtp_password_;

            std::string telegram_token_;
        };
}
#endif
