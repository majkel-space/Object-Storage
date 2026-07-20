#include <boost/asio.hpp>
#include <memory>
#include "connection_handler.hpp"

class Server
{
    using tcpip = boost::asio::ip::tcp;
  public:
    Server(boost::asio::io_service& io_service);

    void StopServer();

  private:
    void StartAccept();

    tcpip::acceptor acceptor_;
    bool stop_ = false;
    std::shared_ptr<StorageManager> storage_manager_;
};
