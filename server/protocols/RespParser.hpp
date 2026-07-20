#ifndef RESP_PARSER
#define RESP_PARSER

#include <iostream>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "IParser.hpp"


class RespParser : public IParser
{
  public:
    struct RespRequest
    {
        std::string method;
        std::string path;
        std::string object;
        std::vector<std::string> arguments;
        std::size_t content_length{0};

        friend std::ostream& operator<<(std::ostream& os, const RespRequest& request)
        {
            os << "RespRequest {\n"
            << "  Request type: " << request.method << "\n"
            << "  Path: " << request.path << "\n"
            << "  Object: " << request.object << "\n"
            << "  Arguments: [";

            for (std::size_t i = 0; i < request.arguments.size(); ++i)
            {
                os << request.arguments[i];
                if (i + 1 < request.arguments.size())
                    os << ", ";
            }
            os << "]\n"
            << "  Content-Length: " << request.content_length << "\n"
            << "}";

            return os;
        }
    };

    ~RespParser() = default;

    void Parse(const std::string_view) override;
    std::string GetMethod() const override { return request_.method; }
    std::string GetPath() const override { return request_.path; }

  private:
    bool ReadLine(std::string& line);
    void ClearRequest();

    RespRequest request_;
    std::string buffer_;
    std::size_t position_ = 0;
    std::size_t remaining_arrays_ = 0;

    enum class State
    {
        ReadArrayHeader,
        ReadBulkHeader,
        ReadBulkData
    };

    State state_ = State::ReadArrayHeader;
};

#endif //RESP_PARSER
