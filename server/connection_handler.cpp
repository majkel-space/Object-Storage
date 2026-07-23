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
    socket_.async_write_some(
        boost::asio::buffer(write_data_, bytes_recieved_),
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
                }
                else if (storage_status == StorageStatus::Receiving){
                    storage_manager_->Append(request, std::span<const char>(data_.data(), bytes_transferred));
                }
                else if(storage_status == StorageStatus::Sending) {
                    HandleWrite(error, bytes_transferred);
                }
            }
            else if (status == ParseStatus::Error) {
                //TODO close connection?
                parser_.reset();
            }
        }
        DoRead();
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
        if (parser_ != nullptr)
        {
            const auto parser_status = parser_->GetParseStatus();
            const auto storage_status = storage_manager_->GetStatus();
            Request& request = parser_->GetRequest();
            if (storage_status == StorageStatus::NotStarted) {
                storage_manager_->Execute(request);
            }
            else if (storage_status == StorageStatus::Sending ){
                bytes_recieved_ = storage_manager_->Read(request, write_data_);
                for (const auto it: write_data_)
                    std::cout << it;
                std::cout << " size " << write_data_.size() << " bytes " << (int)bytes_recieved_ << std::endl;
            }
            else if (storage_status == StorageStatus::Complete) {
                boost::system::error_code ec;
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
                socket_.close(ec);
                return;
            }
            else if (storage_status == StorageStatus::Receiving) {
                HandleRead(error, bytes_transferred);
            }
            DoWrite();
        }
    }
    else
    {
        std::cerr << "error: " << error.message() << std::endl;
        socket_.close();
    }
}
