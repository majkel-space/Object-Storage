#ifndef HTTP_PARSER
#define HTTP_PARSER

#include <iostream>
#include <string_view>
#include <unordered_map>
#include "IParser.hpp"

class HttpParser : public IParser
{
  public:
    ~HttpParser() = default;

    void Parse(const std::string_view) override;

  private:
    bool CheckIfHeaderNotComplete();
    void CreateHttpRequest();
    void ClearRequest();

    std::string buffer_;
    std::unordered_map<std::string, std::string> headers;
    const std::string ending{"\r\n\r\n"};
};

#endif //HTTP_PARSER
