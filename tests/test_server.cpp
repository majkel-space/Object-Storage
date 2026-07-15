#include <gtest/gtest.h>
#include "protocols/ProtocolDetection.hpp"
#include "protocols/HttpParser.hpp"
#include "protocols/RespParser.hpp"

namespace helpers
{
    const std::string http_msg =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: TestClient\r\n"
        "Accept: */*\r\n"
        "\r\n";
    const std::string resp_msg = "+OK\r\n";
    const std::string fail_http_msg =
        "GETHTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: TestClient\r\n"
        "Accept: */*\r\n"
        "\r\n";
    const std::string empty_msg = "";
    const std::string http_msg_begin = "GET / HTTP/1.1\r\n";
    const std::string http_msg_end = "\r\n\r\n";
    const std::vector<std::string> resp_msg_chunks {
        "*2\r\n",
        "$3\r\n",
        "GET\r\n",
        "$5\r\n",
        "hello\r\n"
    };
}

using namespace helpers;

TEST(TestProtocolDetection, ExpectTestDetectHttpAndRestProtocol)
{
    EXPECT_EQ(DetectProtocol(http_msg), Protocol::Http);
    EXPECT_EQ(DetectProtocol(resp_msg), Protocol::Resp);
    EXPECT_EQ(DetectProtocol(fail_http_msg), Protocol::Unknown);
    EXPECT_EQ(DetectProtocol(empty_msg), Protocol::Unknown);
}


TEST(TestHttpParser, ExpectTestDetectHttpAndCheckStatusOfParse)
{
    EXPECT_EQ(DetectProtocol(http_msg), Protocol::Http);
    const auto parser = std::make_unique<HttpParser>();

    EXPECT_EQ(parser->Parse(http_msg_begin), ParseStatus::InProgress);
    EXPECT_EQ(parser->Parse(http_msg_end), ParseStatus::Complete);
}

TEST(TestHttpParser, ExpectTestDetectRespAndCheckStatusOfParse)
{
    EXPECT_EQ(DetectProtocol(resp_msg_chunks.front()), Protocol::Resp);
    const auto parser = std::make_unique<RespParser>();

    for (int it = 0; it < resp_msg_chunks.size() - 1; ++it)
        EXPECT_EQ(parser->Parse(resp_msg_chunks.at(it)), ParseStatus::InProgress);
    EXPECT_EQ(parser->Parse(resp_msg_chunks.back()), ParseStatus::Complete);
}
