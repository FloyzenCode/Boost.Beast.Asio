#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

std::vector<std::string> messages = {"Hello", "World"};

const std::string msgInAPI = "You inside in API";
const std::string msgNotFound = "Not found";

std::string json_array(const std::vector<std::string> &messages)
{
    boost::property_tree::ptree tree;
    for (const auto &message : messages)
    {
        boost::property_tree::ptree message_node;
        message_node.put("", message);
        tree.push_back(std::make_pair("", message_node));
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

    if (req.method() == http::verb::get && req.target() == "/messages")
    {
        res.body() = json_array(messages);
    }
    else if (req.method() == http::verb::post && req.target() == "/messages")
    {
        auto new_messages = parse_json_array(req.body());
        messages.insert(messages.end(), new_messages.begin(), new_messages.end());
        res.result(http::status::created);
        res.body() = "Messages created";
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