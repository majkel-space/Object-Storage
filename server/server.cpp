#include <boost/bind/bind.hpp>
#include "server.hpp"

using tcpip = boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& io_service) : acceptor_(io_service, tcpip::endpoint(tcpip::v4(), 1234))
{
    storage_manager_ = std::make_shared<StorageManager>();
    StartAccept();
}

void Server::StopServer()
{
    stop_ = true;
    boost::system::error_code ec;
    acceptor_.close(ec);
}

void Server::StartAccept()
{
    std::shared_ptr<tcpip::socket> socket = std::make_shared<tcpip::socket>(acceptor_.get_executor());

    acceptor_.async_accept(
        *socket,
        [this, socket](const boost::system::error_code& ec)
        {
            if (!ec)
            {
                //separate connetion handler for each conecting client
                auto connection = std::make_shared<ConnectionHandler>(std::move(*socket), storage_manager_);
                //TODO pass storageManager instance
                connection->Start();
            }
            if (!stop_)
                StartAccept();
        });
}
