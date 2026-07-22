#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace
{
    const std::vector<std::string> http_msg_chunks {
        "PUT aaa HT",
        "TP/1.1\r\n",
        "Host: localhost\r\n",
        "User-Agent: TestClient\r\n",
        "Accept: */*\r\n",
        "Content-Length: 17\r\n",
        "\r\n+++",
        "Hello world",
        "!!!"
    };

    const std::vector<std::string> resp_msg_chunks {
        "*3\r\n",
        "$5\r\n",
        "SETNX\r\n",
        "$4\r\n",
        "resp\r\n",
        "$13\r\n",
        "Resp object\r\n"
    };

    const std::vector<std::string> unknown_msg_chunks {
        "THIS ",
        "IS ",
        "UNKNOWN ",
        "DATA\r\n"
    };
} //namespace

class Client
{
    using tcpip = boost::asio::ip::tcp;
  public:
    Client (boost::asio::io_context&, const int);

    void Connect();
    void SendMessage();

  private:
    void GenerateMessage();
    void GetResponse();

    int id_;
    std::vector<std::string> msg_chunks_;
    tcpip::socket socket_;
    boost::system::error_code error_;
};
