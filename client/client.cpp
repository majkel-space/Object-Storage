#include "client.hpp"



void Client::Connect()
{
    socket_.connect(tcpip::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));
}

void Client::SendMessage()
{
    GenerateMessage();
    GetResponse();
}

void Client::GenerateMessage()
{
    const char msg[] =
    "GET / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: TestClient\r\n"
    "Accept: */*\r\n"
    "\r\n";

    boost::asio::write(socket_, boost::asio::buffer(msg), error_ );
    if(!error_ )
    {
        std::cout << "Client sent hello message!" << std::endl;
    }
    else
    {
        std::cout << "send failed: " << error_.message() << std::endl;
        return;
    }
}

void Client::GetResponse()
{
    boost::asio::streambuf receive_buffer;
    std::size_t bytes_transferred = boost::asio::read_until(socket_, receive_buffer, '\n', error_);

    if (error_ && error_ != boost::asio::error::eof)
    {
        std::cout << "receive failed: " << error_.message() << std::endl;
    }
    else
    {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
        std::cout.write(data, bytes_transferred);
        std::cout << '\n';
    }
}
