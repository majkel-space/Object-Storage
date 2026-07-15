#ifndef I_PARSER
#define I_PARSER

#include <string_view>

enum class ParseStatus
{
    InProgress,
    Complete,
    Error,
};

class IParser
{
  public:
    virtual ~IParser() = default;
    
    virtual ParseStatus Parse(const std::string_view) = 0;
};

#endif //I_PARSER
