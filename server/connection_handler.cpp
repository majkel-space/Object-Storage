#include <boost/bind/bind.hpp>
#include <span>
#include "connection_handler.hpp"
#include "protocols/ProtocolDetection.hpp"
#include "storage/storage.hpp"

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
    std::string message_ = "DoWrite\n";
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
        std::string_view msg(data_.data(), bytes_transferred);
        if (!parser_)
        {
            const Protocol protocol = DetectProtocol(msg);
            switch (protocol)
            {
                case Protocol::Http:
                    parser_ = std::make_unique<HttpParser>();
                    break;

                case Protocol::Resp:
                    parser_ = std::make_unique<RespParser>();
                    break;
            }
        }
        if (parser_ != nullptr)
        {
            auto status = parser_->GetParseStatus();
            if (status != ParseStatus::Complete and status != ParseStatus::Error) {
                parser_->Parse(msg);
                status = parser_->GetParseStatus();
            }
            if (status == ParseStatus::Complete) {
                Request& request = parser_->GetRequest();
                StorageStatus storage_status = storage_manager_->GetStatus();
                if (storage_status == StorageStatus::NotStarted ||
                    storage_status == StorageStatus::Complete) {
                        storage_manager_->Execute(request);
                        // storage_manager_->Append(request, std::span<const char>(data_.data(), bytes_transferred));
                    }
                //TODO check error status from StorageManager
                else if (storage_status == StorageStatus::Receiving){
                    storage_manager_->Append(request, std::span<const char>(data_.data(), bytes_transferred));
                }
                // parser_.reset(); //TODO reset only if full content will be send
            }
            else if (status == ParseStatus::Error) {
                //TODO close connection?
                parser_.reset();
            }
        }
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
        DoRead();
    }
    else
    {
        std::cerr << "error: " << error.message() << std::endl;
        socket_.close();
    }
}
