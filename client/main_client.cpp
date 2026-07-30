#include "client.hpp"

const int NUMBER_OF_CLIENTS = 5U;

int main()
{
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT);
    signals.async_wait(
        [&](const boost::system::error_code&, int)
        {
            std::cout << "Stopping client\n";
            io_context.stop();
        });

    // std::vector<std::shared_ptr<Client>> clients;
    // clients.reserve(NUMBER_OF_CLIENTS);
    // for (auto it = 0U; it < NUMBER_OF_CLIENTS; ++it)
    // {
    //     auto client = std::make_shared<Client>(io_context, static_cast<int>(it + 1));
    //     client->Connect();
    //     clients.push_back(client);
    // }

    auto client = std::make_shared<Client>(io_context, static_cast<int>(1), "127.0.0.1", 1234U);
    client->Connect();

    boost::asio::steady_timer timer(io_context);
    std::function<void()> send_loop;

    // send_loop = [&timer, &clients, &send_loop]()
    // send_loop = [&timer, &client, &send_loop]()
    // {
    //     client->SendMessage(ClientMsgs::http_msg_chunks);

    //     timer.expires_after(std::chrono::seconds(1));
    //     timer.async_wait(
    //         [&](const boost::system::error_code&)
    //         {
    //             send_loop();
    //         });
    // };

    // send_loop();
    client->SendMessage(ClientMsgs::http_msg_chunks);
    client->PrintResponse();
    io_context.run();

    return 0;
}
