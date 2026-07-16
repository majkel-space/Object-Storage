#include <gtest/gtest.h>
#include "helpers.hpp"
#include "protocols/ProtocolDetection.hpp"
#include "protocols/HttpParser.hpp"
#include "protocols/RespParser.hpp"

using namespace helpers;

TEST(TestProtocolDetection, ExpectTestDetectHttpAndRestProtocol)
{
    EXPECT_EQ(DetectProtocol(http_msg), Protocol::Http);
    EXPECT_EQ(DetectProtocol(resp_msg), Protocol::Resp);
    EXPECT_EQ(DetectProtocol(fail_http_msg), Protocol::Unknown);
    EXPECT_EQ(DetectProtocol(empty_msg), Protocol::Unknown);
}

class ParsersTest : public ::testing::Test
{
  protected:
    std::unique_ptr<IParser> parser{nullptr};
};

TEST_F(ParsersTest, ExpectTestDetectHttpAndCheckStatusOfParse)
{
    EXPECT_EQ(DetectProtocol(http_msg), Protocol::Http);
    parser = std::make_unique<HttpParser>();
    parser->Parse(http_msg_begin);
    EXPECT_EQ(parser->GetParseStatus(), ParseStatus::InProgress);
    parser->Parse(http_msg_end);
    EXPECT_EQ(parser->GetParseStatus(), ParseStatus::Complete);
}

TEST_F(ParsersTest, ExpectTestDetectRespAndCheckStatusOfParse)
{
    EXPECT_EQ(DetectProtocol(resp_msg_chunks.front()), Protocol::Resp);
    parser = std::make_unique<RespParser>();

    for (int it = 0; it < resp_msg_chunks.size() - 1; ++it)
    {
        parser->Parse(resp_msg_chunks.at(it));
        EXPECT_EQ(parser->GetParseStatus(), ParseStatus::InProgress);
    }
    parser->Parse(resp_msg_chunks.back());
    EXPECT_EQ(parser->GetParseStatus(), ParseStatus::Complete);

    //TODO when IParser will have details about request test them
}
