#include "notificator.hpp"
#include <nlohmann/json.hpp>
#include <tgbot/tgbot.h>
// Для отправки почты через SMTP
#include <mailio/message.hpp>
#include <mailio/mime.hpp>
#include <mailio/smtp.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

#include <iostream>

using namespace s21;

Notificator::Notificator() {
  last_sentEmail = std::chrono::steady_clock::now() - std::chrono::hours(24);
  last_sentTg = std::chrono::steady_clock::now() - std::chrono::hours(24);
  cooldown = 300s;

  std::ifstream config_file("include/mail.conf");
  if (!config_file.is_open()) {
    printf("No config!!!!");
    password = "";
    token = "";
    gmail = "";
    receiver = "";
    chat_id = "";
    return;
  }
  nlohmann::json config;
  config_file >> config;
  password = config["password"];
  token = config["token"];
  gmail = config["gmail"];
  receiver = config["receiver"];
  chat_id = config["chat_id"];
}

int Notificator::sendEmail(std::string subject, std::string body) {
  auto now = std::chrono::steady_clock::now();
  if (now - last_sentEmail < cooldown)
    return 0;
  try {
    mailio::message msg;
    msg.from(mailio::mail_address("Monitoring System", this->gmail));
    msg.add_recipient(mailio::mail_address("Receiver", this->receiver));
    msg.subject(subject);
    msg.content(body);

    mailio::smtp conn("smtp.gmail.com", 587);
    conn.authenticate(this->gmail, this->password,
                      mailio::smtp::auth_method_t::LOGIN);
    conn.submit(msg);
  } catch (const mailio::smtp_error &error) {
    std::cerr << "Email delivery failed: " << error.what() << '\n';
    return -1;
  }
  last_sentEmail = std::chrono::steady_clock::now();

  return 0;
}

int Notificator::sendTelegram(std::string message) {
  auto now = std::chrono::steady_clock::now();
  if (now - last_sentTg < cooldown)
    return 0;
  TgBot::Bot bot(token);
  bot.getApi().sendMessage(chat_id, message);
  last_sentTg = std::chrono::steady_clock::now();
  return 0;
}