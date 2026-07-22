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
    ~RespParser() = default;

    void Parse(const std::string_view) override;

  private:
    bool ReadLine(std::string& line);
    void ClearRequest();

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
