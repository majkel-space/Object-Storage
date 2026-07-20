#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace
{
    const std::vector<std::string> http_msg_chunks {
        "GET / HT",
        "TP/1.1\r\n",
        "Host: localhost\r\n",
        "User-Agent: TestClient\r\n",
        "Accept: */*\r\n",
        "Content-Length: 11\r\n",
        "\r\n",
        "Hello world"
    };

    const std::vector<std::string> resp_msg_chunks {
        "*3\r\n",
        "$3\r\n",
        "GET\r\n",
        "$9\r\n",
        "resp_path\r\n",
        "$11\r\n",
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
