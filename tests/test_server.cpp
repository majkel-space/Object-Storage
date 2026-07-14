#include <gtest/gtest.h>
#include "protocols/ProtocolDetection.hpp"

TEST(TestProtocolDetection, ExpectTestDetectHttpAndRestProtocol)
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

    EXPECT_EQ(DetectProtocol(http_msg), Protocol::Http);
    EXPECT_EQ(DetectProtocol(resp_msg), Protocol::Resp);
    EXPECT_EQ(DetectProtocol(fail_http_msg), Protocol::Unknown);
    EXPECT_EQ(DetectProtocol(empty_msg), Protocol::Unknown);
}
