#include <boost/bind/bind.hpp>
#include "connection_handler.hpp"
#include "protocols/ProtocolDetection.hpp"

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
        std::string msg(data_.data(), bytes_transferred);
        const Protocol protocol = DetectProtocol(msg);
        if (protocol == Protocol::Http)
            std::cout << "HTTTP msg " << msg << std::endl;
        else if (protocol == Protocol::Resp)
            std::cout << "RESP msg " << msg << std::endl;
        else
            std::cout <<"Protocol Unknown\n";

        DoWrite();
    }
    else
    {
        std::cerr << "error: " << error.message() << std::endl;
        socket_.close();
    }
}

void ConnectionHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        std::cout << "Server sent Hello message!"<< std::endl;
        DoRead();
    }
    else
    {
        std::cerr << "error: " << error.message() << std::endl;
        socket_.close();
    }
}
