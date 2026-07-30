#include <boost/asio.hpp>
#include <memory>
#include "connection_handler.hpp"

class Server
{
    using tcpip = boost::asio::ip::tcp;
  public:
    Server(boost::asio::io_context& io_context, std::uint16_t port = 1234, const std::string& storage_path = "../output/")
      : acceptor_(io_context, tcpip::endpoint(tcpip::v4(), port))
    {
        storage_manager_ = std::make_shared<StorageManager>(storage_path);
        StartAccept();
    }

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;
    ~Server() = default;

    void StopServer();
    std::uint16_t GerServerPort() const { return acceptor_.local_endpoint().port(); }

  private:
    void StartAccept();

    tcpip::acceptor acceptor_;
    bool stop_ = false;
    std::shared_ptr<StorageManager> storage_manager_;
};
