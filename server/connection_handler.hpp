#include <array>
#include <boost/asio.hpp>
#include <memory>
#include "protocols/HttpParser.hpp"
#include "protocols/RespParser.hpp"
#include "storage/storage_manager.hpp"

const int max_length = 1024 ;

class ConnectionHandler : public std::enable_shared_from_this<ConnectionHandler>
{
  public:
    using tcpip = boost::asio::ip::tcp;

    ConnectionHandler(boost::asio::ip::tcp::socket socket, std::shared_ptr<StorageManager> storage_manager)
      : socket_(std::move(socket)),  storage_manager_(std::move(storage_manager)) {}

    void Start();

  private:
    void DoRead();
    void DoWrite();
    void HandleRead(const boost::system::error_code&, size_t);
    void HandleWrite(const boost::system::error_code&, size_t);

    tcpip::socket socket_;
    std::array<char, 1024> data_; //no new allocation, every read use the same memory
    std::array<char, 1024> write_data_;
    std::unique_ptr<IParser> parser_{nullptr};
    std::shared_ptr<StorageManager> storage_manager_;
    std::size_t bytes_recieved_;
};
