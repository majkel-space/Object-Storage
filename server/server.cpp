#include <boost/bind/bind.hpp>
#include "server.hpp"

using tcpip = boost::asio::ip::tcp;

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
                connection->Start();
            }
            if (!stop_)
                StartAccept();
        });
}
