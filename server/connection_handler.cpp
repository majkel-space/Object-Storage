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
        boost::asio::buffer(read_data_),
        boost::bind(&ConnectionHandler::HandleRead,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void ConnectionHandler::DoWrite()
{
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(write_data_, bytes_recieved_),
        boost::bind(&ConnectionHandler::HandleWrite,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void ConnectionHandler::HandleWriteCompletion(
    const boost::system::error_code& error, size_t bytes_transferred, const std::string& response)
{
    closing_response_ = response;
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(closing_response_),
        boost::bind(&ConnectionHandler::HandleWrite,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void ConnectionHandler::HandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        if (!parser_)
        {
            std::string_view msg(read_data_.data(), bytes_transferred);
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
            if (status != HeaderParseStatus::Complete and status != HeaderParseStatus::Error) {
                std::string msg(read_data_.data(), bytes_transferred);
                parser_->Parse(msg);
                status = parser_->GetParseStatus();
            }
            if (status == HeaderParseStatus::Complete) {
                Request& request = parser_->GetRequest();
                MessageStatus& storage_status = request.msg_status;
                if (storage_status == MessageStatus::NotStarted) {
                    storage_manager_->Execute(request);
                }
                else if (storage_status == MessageStatus::Receiving){
                    storage_manager_->Append(request, std::span<const char>(read_data_.data(), bytes_transferred));
                }
                if(storage_status == MessageStatus::Sending) {
                    HandleWrite(error, bytes_transferred);
                }
                //Message Error also generate final_reponse_
                if (storage_status == MessageStatus::FinalResponse || storage_status == MessageStatus::Error) {
                    GenerateFinalResponse(request);
                }
                if (storage_status == MessageStatus::Complete) {
                    boost::system::error_code ec;
                    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_receive, ec);
                }
            }
            DoRead();
        }
    }
    else
    {
        boost::system::error_code ec;
        socket_.close(ec);
    }
}

void ConnectionHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        if (parser_ != nullptr)
        {
            Request& request = parser_->GetRequest();
            const auto parser_status = parser_->GetParseStatus();
            auto& storage_status = request.msg_status;
            if (storage_status == MessageStatus::NotStarted) {
                storage_manager_->Execute(request);
            }
            else if (request.operation == Operation::List && request.msg_status == MessageStatus::Sending) {
                storage_status = MessageStatus::FinalResponse;
                HandleWriteCompletion(error, request.final_response.size(), request.final_response);
            }
            else if (storage_status == MessageStatus::Sending ){
                bytes_recieved_ = storage_manager_->Read(request, write_data_);
            }
            else if (storage_status == MessageStatus::Receiving) {
                HandleRead(error, bytes_transferred);
            }
            if (storage_status == MessageStatus::FinalResponse || storage_status == MessageStatus::Error) {
                GenerateFinalResponse(request);
            }

            if (storage_status == MessageStatus::Complete) {
                boost::system::error_code ec;
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
            }
            else
                DoWrite();
        }
    }
    else
    {
        boost::system::error_code ec;
        socket_.close(ec);
    }
}

void ConnectionHandler::GenerateFinalResponse(Request& request)
{
    boost::system::error_code ec;
    boost::asio::write(socket_, boost::asio::buffer(request.final_response), ec);
    request.msg_status = MessageStatus::Complete;
}
