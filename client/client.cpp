#include <thread>
#include "client.hpp"

Client::Client (boost::asio::io_context& io, const int id) : socket_(io), id_(id)
{
    if (id_ == 1 || id_ == 3)
        msg_chunks_ = http_msg_chunks;
    else if (id_ == 2 || id_ == 4)
        msg_chunks_ = resp_msg_chunks;
    else
        msg_chunks_ = unknown_msg_chunks;
}

void Client::Connect()
{
    socket_.connect(tcpip::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234 ));
}

void Client::SendMessage()
{
    GenerateMessage();
    // GetResponse();
}

void Client::GenerateMessage()
{
    for (const auto& chunk: msg_chunks_)
    {
        //blocking write, with larger data change to async_write, to not block
        boost::asio::write(socket_, boost::asio::buffer(chunk), error_ );
        if(!error_ )
        {
            std::cout << "Client " << id_ << " sent chunk: " << chunk << '\n';
        }
        else
        {
            std::cout << "Client " << id_ << " send failed: " << error_.message() << '\n';
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
