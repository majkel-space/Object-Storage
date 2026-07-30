#ifndef I_PARSER
#define I_PARSER

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>

enum class HeaderParseStatus
{
    NotStarted,
    InProgress,
    Complete,
    Error,
};

enum class Operation
{
    None,
    List,
    Put,
    PutX,
    Get,
    Unknown
};

enum class MessageStatus
{
    NotStarted,
    Receiving,
    Sending,
    FinalResponse,
    Complete,
    Error,
};


struct Request
{
    std::string method;
    std::string path_str;
    std::string version;
    std::size_t content_length{0};
    std::vector<char> body;
    Operation operation;
    MessageStatus msg_status;
    std::filesystem::path path;
    std::filesystem::path tmp_path;
    std::ofstream write_file;
    std::ifstream read_file;
    std::string final_response;

    friend std::ostream& operator<<(std::ostream& os, const Request& request)
    {
        os << "Request {\n"
          << "  Method: " << request.method << "\n"
          << "  Target: " << request.path_str << "\n"
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
    HeaderParseStatus GetParseStatus() const { return parse_status_; }
    Request& GetRequest() { return request_; }

  protected:
    HeaderParseStatus parse_status_{HeaderParseStatus::NotStarted};
    Request request_;
};

#endif //I_PARSER
