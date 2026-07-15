#include <string>
#include <vector>
#include "IParser.hpp"

class RespParser : public IParser
{
  public:
    ~RespParser() = default;

    ParseStatus Parse(const std::string_view) override;

  private:
    bool ReadLine(std::string& line);

    std::string buffer_;
    std::size_t position_ = 0;
    std::size_t remaining_arrays_ = 0;
    std::size_t remaining_bytes_ = 0;

    enum class State
    {
        ReadArrayHeader,
        ReadBulkHeader,
        ReadBulkData
    };

    State state_ = State::ReadArrayHeader;
    std::vector<std::string> arguments_;

};
