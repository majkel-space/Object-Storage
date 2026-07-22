#ifndef I_PARSER
#define I_PARSER

#include <iostream>
#include <string_view>
#include <vector>

enum class ParseStatus
{
    NotStarted,
    InProgress,
    Complete,
    Error,
};


struct Request
{
    std::string method;
    std::string path;
    std::string version;
    std::size_t content_length{0};
    std::vector<char> body;

    friend std::ostream& operator<<(std::ostream& os, const Request& request)
    {
        os << "Request {\n"
          << "  Method: " << request.method << "\n"
          << "  Target: " << request.path << "\n"
          << "  Version: " << request.version << "\n"
          << "  Content-Length: " << request.content_length << "\n"
          << "  Headers:\n";
        os << " Body: ";
        for (const auto& byte: request.body)
            os << byte;
        os <<  "\n" << "}\n";
        return os;
    }
};

class IParser
{
  public:
    virtual ~IParser() = default;

    virtual void Parse(const std::string_view) = 0;
    ParseStatus GetParseStatus() const { return parse_status_; }
    Request& GetRequest() { return request_; }

  protected:
    ParseStatus parse_status_{ParseStatus::NotStarted};
    Request request_;
};

#endif //I_PARSER
