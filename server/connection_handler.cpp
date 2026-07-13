#include <boost/bind/bind.hpp>
#include "connection_handler.hpp"

using tcpip = boost::asio::ip::tcp;

void ConnectionHandler::Start()
{
    DoRead();
}

void ConnectionHandler::DoRead()
{
    socket_.async_read_some(
        boost::asio::buffer(data_),
        boost::bind(&ConnectionHandler::HandleRead,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void ConnectionHandler::DoWrite()
{
    socket_.async_write_some(
        boost::asio::buffer(message_),
        boost::bind(&ConnectionHandler::HandleWrite,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void ConnectionHandler::HandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        for (const auto& c: data_)
            std::cout << c; //it is printing emtpy bytes up to data array capacity
        std::cout << std::endl;
    }
    else
    {
        std::cerr << "error: " << error.message() << std::endl;
        socket_.close();
    }
    DoWrite();
}

void ConnectionHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        std::cout << "Server sent Hello message!"<< std::endl;
    }
    else
    {
        std::cerr << "error: " << error.message() << std::endl;
        socket_.close();
    }
    DoRead();
}
