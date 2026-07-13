#include <boost/asio.hpp>
#include <iostream>
#include <string>

class Client
{
    using tcpip = boost::asio::ip::tcp;
  public:
    Client (boost::asio::io_context& io) : socket_(io) {}

    void Connect();
    void SendMessage();

  private:
    void GenerateMessage();
    void GetResponse();

    tcpip::socket socket_;
    boost::system::error_code error_;
};
