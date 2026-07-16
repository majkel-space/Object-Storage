#ifndef HTTP_PARSER
#define HTTP_PARSER

#include <iostream>
#include <string_view>
#include <unordered_map>
#include "IParser.hpp"

class HttpParser : public IParser
{
  public:
    struct HttpRequest
    {
        std::string method;
        std::string target;
        std::string version;
        std::string path;
        std::unordered_map<std::string, std::string> headers;
        std::size_t content_length{0};
        std::string body;

        friend std::ostream& operator<<(std::ostream& os, const HttpRequest& request)
        {
            os << "HttpRequest {\n"
              << "  Method: " << request.method << "\n"
              << "  Target: " << request.target << "\n"
              << "  Version: " << request.version << "\n"
              << "  Path: " << request.path << "\n"
              << "  Content-Length: " << request.content_length << "\n"
              << "  Headers:\n";

            for (const auto& [key, value] : request.headers)
            {
                os << "    " << key << ": " << value << "\n";
            }

            os << "  Body: " << request.body << "\n"
              << "}\n";

            return os;
        }
    };

    ~HttpParser() = default;

    void Parse(const std::string_view) override;
    std::string_view GetPath() const override { return request_.path; }

  private:
    bool CheckIfHeaderNotComplete();
    void CreateHttpRequest();
    void ClearRequest();

    HttpRequest request_;
    std::string buffer_;
    const std::string ending{"\r\n\r\n"};
};

#endif //HTTP_PARSER
