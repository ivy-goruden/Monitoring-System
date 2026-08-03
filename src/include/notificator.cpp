#include "notificator.hpp"
#include <nlohmann/json.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <stdexcept>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>   // для tcp_stream
#include <boost/beast/http.hpp>              // для http::request/response
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

#include <iostream>

using namespace s21;

Notificator::Notificator(){
    smtp_server_ = "mail.example.com";
    smtp_port_ = 587;
    smtp_username_ = "me@example.com";
    smtp_password_ = "password";
}

int Notificator::sendEmail(std::string subject, std::string body, std::string email){
    Poco::Net::MailMessage msg;
    msg.addRecipient(Poco::Net::MailRecipient (Poco::Net::MailRecipient::PRIMARY_RECIPIENT,
                                        email, "recipient"));
    msg.setSender("Monitoring system");
    msg.setSubject(subject);
    msg.setContent(body);

    Poco::Net::SMTPClientSession smtp(smtp_server_, smtp_port_);
    smtp.login(smtp_server_, Poco::Net::SMTPClientSession::AUTH_LOGIN, smtp_username_, smtp_password_);
    smtp.sendMessage(msg);
    smtp.close();
    return 0;
}

int Notificator::sendTelegram(std::string message, std::string chat_id){
    // Создаем JSON-объект с данными для отправки
    nlohmann::json payload;
    payload["chat_id"] = chat_id;
    payload["text"] = message;
    try {
        boost::asio::io_context ioc;
        boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);
        ssl_ctx.set_default_verify_paths();
        tcp::resolver resolver(ioc);

        boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ioc, ssl_ctx);

        // Разрешаем доменное имя и устанавливаем соединение
        auto const results = resolver.resolve("api.telegram.org", "443");
        boost::beast::get_lowest_layer(stream).connect(results);

        // Выполняется SSL-рукопожатие для установки защищенного соединения.
        stream.handshake(boost::asio::ssl::stream_base::client);

        // Создаем HTTP-запрос
        http::request<http::string_body> req{http::verb::post, "/bot" + telegram_token_ + "/" + "/sendMessage", 11};
        req.set(http::field::host, "api.telegram.org");
        req.set(http::field::content_type, "application/json");
        req.body() = payload.dump();
        req.prepare_payload();

        // Отправляем запрос
        http::write(stream, req);
     // Получаем ответ
        boost::beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        // Закрываем SSL-соединение
        boost::system::error_code ec;
        stream.shutdown(ec);

        if (ec == boost::asio::ssl::error::stream_truncated) {
            ec.assign(0, ec.category());
        } else if (ec) {
            throw boost::system::system_error(ec);
        }

        // Парсим и возвращаем ответ в формате JSON
        return nlohmann::json::parse(res.body());
    } catch (const std::exception& e) {
        // Обрабатываем ошибки
        throw std::runtime_error(std::string("Request failed: ") + e.what());
    }
    return 0;
}