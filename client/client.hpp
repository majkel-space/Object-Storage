#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace ClientMsgs
{
    const std::vector<std::string> http_msg_chunks {
        "GET / HT",
        "TP/1.1\r\n",
        "Host: localhost\r\n",
        "User-Agent: TestClient\r\n",
        "Accept: */*\r\n",
        "Content-Length: 16\r\n",
        "\r\n+++",
        "Hello http",
        "!!!"
    };

    const std::vector<std::string> resp_set_msg_chunks {
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
} //namespace ClientMsgs

class Client
{
    using tcpip = boost::asio::ip::tcp;
  public:
    Client (boost::asio::io_context&, const int, std::string host, std::uint16_t);

    void Connect();
    void SendMessage(const std::vector<std::string>&);
    void SendMessage(const std::string);
    void SendObject(const std::string path);
    void PrintResponse();
    std::string GetResponse();
    void CloseConnection();

  private:
    int id_;
    std::string write_buffer_;
    tcpip::socket socket_;
    std::string host_;
    std::uint16_t port_;
    boost::system::error_code error_;
};
