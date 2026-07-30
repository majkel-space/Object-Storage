#include <fstream>
#include <thread>
#include "client.hpp"

Client::Client (boost::asio::io_context& io, const int id, std::string host, std::uint16_t port)
    : socket_(io), id_(id), host_(std::move(host)), port_(port) {}

void Client::Connect()
{
    socket_.connect(tcpip::endpoint(boost::asio::ip::address::from_string(host_), port_ ));
    // auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(host_), port_);
    // socket_.async_connect(endpoint,
    //     [this](const boost::system::error_code& ec)
    //     {
    //         if (ec)
    //         {
    //             std::cout << "Connect failed: " << ec.message() << '\n';
    //             return;
    //         }
    //     });
}

void Client::SendMessage(const std::vector<std::string>& msg_chunks)
{
    boost::system::error_code ec;
    for (const auto chunk: msg_chunks)
    {
        boost::asio::write(socket_, boost::asio::buffer(chunk), ec);
        if (ec)
            std::cout << "Client send failed: " << ec.message() << std::endl;
    }
}

void Client::SendMessage(const std::string msg)
{
    boost::system::error_code ec;
    boost::asio::write(socket_, boost::asio::buffer(msg), ec);
        if (ec)
            std::cout << "Client send failed: " << ec.message() << std::endl;
}

void Client::SendObject(const std::string path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open file: " + path);

    std::array<char, 1024> buffer;
    boost::system::error_code ec;

    while (file)
    {
        file.read(buffer.data(), buffer.size());
        const auto bytes_read = file.gcount();

        if (bytes_read > 0)
        {
            boost::asio::write(socket_, boost::asio::buffer(buffer.data(), bytes_read), ec);
            if (ec)
                std::cout << "Client send failed: " << ec.message() << std::endl;
        }
    }
}

void Client::PrintResponse()
{
    boost::asio::streambuf receive_buffer;
    std::size_t bytes_transferred = boost::asio::read(socket_, receive_buffer, error_);
    if (error_ && error_ != boost::asio::error::eof)
    {
        std::cerr << "receive failed: " << error_.message() << std::endl;
    }
    else
    {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
        std::cout.write(data, bytes_transferred);
        std::cout << '\n';
        receive_buffer.consume(bytes_transferred);
    }
}

std::string Client::GetResponse()
{
    boost::asio::streambuf receive_buffer;
    std::size_t bytes_transferred = boost::asio::read(socket_, receive_buffer, error_);
    if (error_ && error_ != boost::asio::error::eof)
    {
        throw boost::system::system_error(error_);
    }

    return {
        boost::asio::buffers_begin(receive_buffer.data()),
        boost::asio::buffers_begin(receive_buffer.data()) + bytes_transferred
    };
}

void Client::CloseConnection()
{
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);
}
