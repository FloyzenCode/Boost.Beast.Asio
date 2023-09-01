#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

std::vector<std::string> messages = {"Hello", "World"};

std::string json_array(const std::vector<std::string> &arr)
{
    std::stringstream ss;
    ss << '[';
    for (size_t i = 0; i < arr.size(); ++i)
    {
        ss << '"' << arr[i] << '"';
        if (i < arr.size() - 1)
        {
            ss << ',';
        }
    }
    ss << ']';
    return ss.str();
}

void handle_request(http::request<http::string_body> const &req, http::response<http::string_body> &res)
{
    res.set(http::field::server, "Boost.Asio RESTful Server");
    res.set(http::field::content_type, "application/json");

    if (req.method() == http::verb::get && req.target() == "/messages")
    {
        res.body() = json_array(messages);
    }
    else if (req.method() == http::verb::post && req.target() == "/messages")
    {
        messages.emplace_back(req.body());
        res.result(http::status::created);
        res.body() = "Message created";
    }
    else if (req.method() == http::verb::patch && req.target() == "/messages")
    {
        std::string id_str, message;
        std::istringstream req_body_ss(req.body());
        req_body_ss >> id_str >> message;

        int id = std::stoi(id_str);

        if (id >= 0 && id < messages.size())
        {
            messages[id] = message;
            res.body() = "Message updated";
        }
        else
        {
            res.result(http::status::not_found);
            res.body() = "Message not found";
        }
    }
    else if (req.method() == http::verb::delete_ && req.target() == "/messages")
    {
        std::string message_id = std::string(req["Message-Id"]);
        int id = std::stoi(message_id);
        if (id >= 0 && id < messages.size())
        {
            messages.erase(messages.begin() + id);
            res.body() = "Message deleted";
        }
        else
        {
            res.result(http::status::not_found);
            res.body() = "Message not found";
        }
    }
    else
    {
        res.result(http::status::not_found);
        res.body() = "Not found!";
    }

    res.content_length(res.body().size());
}

int main()
{
    try
    {
        asio::io_context ioc;
        asio::ip::port_type port = 5000;
        tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), port));
        std::cout << "Server running at http://localhost:" << port << "/\n";

        for (;;)
        {
            tcp::socket socket(ioc);
            acceptor.accept(socket);
            beast::flat_buffer buffer;
            http::request<http::string_body> request;
            http::read(socket, buffer, request);
            http::response<http::string_body> response;
            handle_request(request, response);
            http::write(socket, response);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}