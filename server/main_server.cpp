#include <csignal>
#include <boost/asio.hpp>
#include <iostream>
#include "server.hpp"

int main()
{
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT);
    Server server(io_context);
    signals.async_wait(
        [&](const boost::system::error_code&, int)
        {
            server.StopServer();
            io_context.stop();
        });
    io_context.run();
    return 0;
}
