#include <filesystem>
#include <future>
#include <gtest/gtest.h>
#include "../client/client.hpp"
#include "../server/server.hpp"
#include "helpers.hpp"

class IntegrationTest : public ::testing::Test
{
  public:
    void SetUp() override
    {
        std::filesystem::remove_all(helpers::test_output_path);
        std::filesystem::create_directories(helpers::test_output_path);

        try
        {
            server_ = std::make_unique<Server>(server_io_, 0U, helpers::test_output_path);
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
            throw;
        }
        server_thread_ = std::thread([this]
        {
            server_io_.run();
        });
    }

    void TearDown() override
    {
        server_io_.stop();
        if (server_thread_.joinable())
            server_thread_.join();
        std::filesystem::remove_all(storage_path_);
    }

  protected:
    std::string SendRequest(const std::string& request, const int id)
    {
        Client client(client_io_, id, "127.0.0.1", server_->GerServerPort());
        client.Connect();
        client.SendMessage(request);
        client_io_.run();
        std::string response = client.GetResponse();
        client.CloseConnection();
        return response;
    }

    std::string SendRequest(const std::vector<std::string>& request_chunks, const int id)
    {
        Client client(client_io_, id, "127.0.0.1", server_->GerServerPort());
        client.Connect();
        client.SendMessage(request_chunks);
        if (client_io_.stopped())
            client_io_.restart();
        client_io_.run();
        std::string response = client.GetResponse();
        client.CloseConnection();
        return response;
    }

    std::string SendObject(const std::string& header, const std::string& file_path, int id)
    {
        boost::asio::io_context io;
        Client client(io, id, "127.0.0.1", server_->GerServerPort());
        client.Connect();
        client.SendMessage(header);
        client.SendObject(file_path);
        auto response = client.GetResponse();
        client.CloseConnection();
        return response;
    }

    boost::asio::io_context server_io_;
    boost::asio::io_context client_io_;
    std::unique_ptr<Server> server_;
    std::thread server_thread_;
    std::filesystem::path storage_path_;
};

TEST_F(IntegrationTest, Test_HttpPut)
{
    auto response = SendRequest(helpers::http_put_msg, 1);
    std::cout << "TEST HTTP response " << response << std::endl;
    //Response of successfull PUT
    EXPECT_TRUE(response.find(helpers::test_output_path) != std::string::npos);

    //Check file existance after successfull PUT
    const std::filesystem::path expected_file = "../test/test_output/aaa";
    EXPECT_TRUE(std::filesystem::exists(expected_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(expected_file));

    auto response_chunk = SendRequest(helpers::http_put_msg_chunks, 2);
    std::cout << "TEST HTTP chunk response " << response_chunk << std::endl;
    // Response of successfull PUT
    EXPECT_TRUE(response_chunk.find(helpers::test_output_path) != std::string::npos);

    // Check file existance after successfull PUT
    const std::filesystem::path expected_file_from_chunk = "../test/test_output/aaa_http_put";
    EXPECT_TRUE(std::filesystem::exists(expected_file_from_chunk));
    EXPECT_TRUE(std::filesystem::is_regular_file(expected_file_from_chunk));
}

TEST_F(IntegrationTest, Test_RespChunkPutAndPutX)
{
    auto response = SendRequest(helpers::resp_set_msg_chunks, 1);
    std::cout << "TEST RESP chunk response " << response << std::endl;
    // Response of successfull PUT
    EXPECT_TRUE(response.find(helpers::test_output_path) != std::string::npos);

    // Check file existance after successfull PUT
    const std::filesystem::path expected_file = "../test/test_output/aaa_resp_obj";
    EXPECT_TRUE(std::filesystem::exists(expected_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(expected_file));

    auto response_setnx = SendRequest(helpers::resp_setnx_msg_chunks, 1);
    std::cout << "TEST RESP chunk response " << response_setnx << std::endl;
    EXPECT_TRUE(response_setnx.find(expected_file.string() + " already exists.") != std::string::npos);
}

TEST_F(IntegrationTest, Test_HttpAndRespGet_NoObject)
{
    auto http_response = SendRequest(helpers::http_get_msg, 1);
    std::cout << "TEST HTTP chunk http_response " << http_response << std::endl;
    // http_response of failed GET
    EXPECT_TRUE(http_response.find("Error: Object " + helpers::test_output_path) != std::string::npos);

    auto resp_response = SendRequest(helpers::resp_get_resp_obj_chunks, 2);
    std::cout << "TEST RESP chunk resp_response " << resp_response << std::endl;
    // resp_response of failed GET
    EXPECT_TRUE(resp_response.find("Error: Object " + helpers::test_output_path) != std::string::npos);
}

TEST_F(IntegrationTest, TestHttpListAndRespKeys)
{
    SendRequest(helpers::http_put_msg_chunks, 2);
    auto http_response =SendRequest(helpers::http_list_msg, 1);
    std::cout << "TEST HTTP chunk http_response " << http_response << std::endl;
    EXPECT_TRUE(http_response.find("aaa_http_put") != std::string::npos);
}

TEST_F(IntegrationTest, Test_HttpAndRespCrossGet)
{
    auto response_http = SendRequest(helpers::http_put_msg_chunks, 2);
    std::cout << "TEST HTTP chunk response " << response_http << std::endl;
    // Response of successfull PUT
    EXPECT_TRUE(response_http.find(helpers::test_output_path) != std::string::npos);

    // Check file existance after successfull PUT
    const std::filesystem::path expected_http_file = "../test/test_output/aaa_http_put";
    EXPECT_TRUE(std::filesystem::exists(expected_http_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(expected_http_file));

    auto response_resp = SendRequest(helpers::resp_set_msg_chunks, 1);
    std::cout << "TEST RESP chunk response " << response_resp << std::endl;
    // Response of successfull PUT
    EXPECT_TRUE(response_resp.find(helpers::test_output_path) != std::string::npos);

    // Check file existance after successfull PUT
    const std::filesystem::path expected_resp_file = "../test/test_output/aaa_resp_obj";
    EXPECT_TRUE(std::filesystem::exists(expected_resp_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(expected_resp_file));

    auto get_http_obj_response = SendRequest(helpers::http_get_http_obj_chunks, 3);
    auto get_resp_obj_response = SendRequest(helpers::http_get_resp_obj_chunks, 4);
    std::cout << "TEST HTTP GET http_obj response " << get_http_obj_response << std::endl;
    std::cout << "TEST HTTP GET resp_obj response " << get_resp_obj_response << std::endl;
    EXPECT_TRUE(get_http_obj_response.find(helpers::test_output_path + "aaa_http_put") != std::string::npos);
    EXPECT_TRUE(get_resp_obj_response.find(helpers::test_output_path + "aaa_resp_obj") != std::string::npos);
}

TEST_F(IntegrationTest, Test_HttpAndRespPutBigObjectToStorage)
{
    //TODO to test, pyt a file into UT utility test location and adapt name of the object "big_file.pdf"
    //TODO size of content will be calculated automatically and put into the request
    const std::string path = "../../tests/utility/big_file.pdf";
    const auto origin_size = (int)std::filesystem::file_size(path);

    const std::string http_put_big_object =
        "PUT http_big_obj HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: " + std::to_string(origin_size) + "\r\n\r\n";
    const std::string resp_big_object =
        "*3\r\n"
        "$3\r\n"
        "SET\r\n"
        "$12\r\n"
        "resp_big_obj\r\n"
        "$"  + std::to_string(origin_size) + "\r\n";

    auto f1 = std::async(std::launch::async, [this, http_put_big_object, path]() {
        return SendObject(http_put_big_object, path, 1);
    });

    auto f2 = std::async(std::launch::async, [this, resp_big_object, path]() {
        return SendObject(resp_big_object, path, 2);
    });

    auto response_http = f1.get();
    auto response_resp = f2.get();

    // auto response_http = SendObject(http_put_big_object, path, 1);
    // auto response_resp = SendObject(resp_big_object, path, 1);
    std::cout << "TEST HTTP big_obj response " << response_http << std::endl;
    std::cout << "TEST RESP big_obj response " << response_resp << std::endl;

    const std::filesystem::path expected_http_file = "../test/test_output/http_big_obj";
    EXPECT_TRUE(std::filesystem::exists(expected_http_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(expected_http_file));
    EXPECT_EQ(origin_size, std::filesystem::file_size(expected_http_file));

    const std::filesystem::path expected_resp_file = "../test/test_output/resp_big_obj";
    EXPECT_TRUE(std::filesystem::exists(expected_resp_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(expected_resp_file));
    EXPECT_EQ(origin_size, std::filesystem::file_size(expected_resp_file));
}
