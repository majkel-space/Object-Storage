#include <csignal>
#include <boost/asio.hpp>
#include <iostream>
#include "server.hpp"

int main()
{
    boost::asio::io_service io_service;
    boost::asio::signal_set signals(io_service, SIGINT);
    Server server(io_service);
    signals.async_wait(
        [&](const boost::system::error_code&, int)
        {
            server.StopServer();
            io_service.stop();
        });
    io_service.run();
    return 0;
}
