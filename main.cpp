#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include "src/routes.cpp"

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