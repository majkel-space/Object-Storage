#include <string>
#include "IParser.hpp"

class HttpParser : public IParser
{
  public:
    ~HttpParser() = default;

    ParseStatus Parse(const std::string_view) override;

  private:
    bool CheckIfdataComplete();

    std::string buffer_;
    const std::string ending{"\r\n\r\n"};
};
