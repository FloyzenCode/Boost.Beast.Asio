#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

struct Message
{
    int id;
    std::string text;
};

std::vector<Message> messages = {
    {1, "Hello"},
    {2, "World"},
};

const std::string msgInAPI = "You inside in API";
const std::string msgNotFound = "Not found";

std::string json_array(const std::vector<Message> &messages)
{
    boost::property_tree::ptree tree;
    for (const auto &message : messages)
    {
        boost::property_tree::ptree message_node;
        message_node.put("id", message.id);
        message_node.put("text", message.text);
        tree.push_back(std::make_pair("first", message_node));
    }

    std::stringstream json_ss;
    boost::property_tree::write_json(json_ss, tree);
    return json_ss.str();
}

// Десериализация json-строки в вектор сообщений
std::vector<std::string> parse_json_array(const std::string &json)
{
    std::vector<std::string> result;
    std::stringstream json_ss(json);

    boost::property_tree::ptree tree;
    boost::property_tree::read_json(json_ss, tree);

    for (const auto &element : tree)
    {
        result.push_back(element.second.get_value<std::string>());
    }

    return result;
}

void handle_request(http::request<http::string_body> const &req, http::response<http::string_body> &res)
{
    res.set(http::field::server, "Boost.Asio RESTful Server");
    res.set(http::field::content_type, "application/json");
    res.set(http::field::access_control_allow_origin, "*");

    if (req.method() == http::verb::get && req.target() == "/messages")
    {
        res.body() = json_array(messages);
    }
    else if (req.method() == http::verb::get && req.target() == "/api")
    {
        res.body() = msgInAPI;
    }
    else
    {
        res.result(http::status::not_found);
        res.body() = msgNotFound; // 404
    }

    res.content_length(res.body().size());
}
int main()
{
    try
    {
        boost::asio::io_context ioc;
        boost::asio::ip::port_type port = 5000;
        asio::ip::tcp::acceptor acceptor(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        std::cout << "Server running at http://localhost:" << port << "/\n";

        for (;;)
        {
            asio::ip::tcp::socket socket(ioc);
            acceptor.accept(socket);
            beast::flat_buffer buffer;
            beast::http::request<beast::http::string_body> request;
            beast::http::read(socket, buffer, request);
            beast::http::response<beast::http::string_body> response;
            handle_request(request, response);
            beast::http::write(socket, response);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}