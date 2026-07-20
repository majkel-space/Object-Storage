#ifndef I_PARSER
#define I_PARSER

#include <string_view>

enum class ParseStatus
{
    NotStarted,
    InProgress,
    Complete,
    Error,
};

class IParser
{
  public:
    virtual ~IParser() = default;

    virtual void Parse(const std::string_view) = 0;
    virtual std::string GetMethod() const = 0;
    virtual std::string GetPath() const = 0;
    ParseStatus GetParseStatus() const { return parse_status_; }

  protected:
    ParseStatus parse_status_{ParseStatus::NotStarted};
};

#endif //I_PARSER
