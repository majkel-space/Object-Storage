#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include "protocols/HttpParser.hpp"
#include "protocols/RespParser.hpp"

const int max_length = 1024 ;

class ConnectionHandler : public std::enable_shared_from_this<ConnectionHandler>
{
  public:
    using tcpip = boost::asio::ip::tcp;

    ConnectionHandler(boost::asio::ip::tcp::socket socket): socket_(std::move(socket)) {}

    void Start();

  private:
    void DoRead();
    void DoWrite();
    void HandleRead(const boost::system::error_code&, size_t);
    void HandleWrite(const boost::system::error_code&, size_t);

    tcpip::socket socket_;
    const char message_[max_length] = "Hello From Server!\n";
    std::array<char, 1024> data_;
    std::unique_ptr<IParser> parser_{nullptr};
};
