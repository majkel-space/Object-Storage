#include <iostream>
#include "RespParser.hpp"

ParseStatus RespParser::Parse(const std::string_view data)
{
    buffer_.append(data);
    while (true)
    {
        switch (state_)
        {
            case State::ReadArrayHeader:
            {
                std::string line;
                if (!ReadLine(line))
                    return ParseStatus::InProgress;

                if (line.empty() || line[0] != '*')
                    return ParseStatus::Error;

                remaining_arrays_ = std::stoul(line.substr(1));
                arguments_.clear();
                state_ = State::ReadBulkHeader;
                break;
            }
            case State::ReadBulkHeader:
            {
                std::string line;
                if (!ReadLine(line))
                    return ParseStatus::InProgress;
                if (line.empty() || line[0] != '$')
                    return ParseStatus::Error;
                remaining_bytes_ = std::stoul(line.substr(1));
                state_ = State::ReadBulkData;
                break;
            }
            case State::ReadBulkData:
            {
                if (buffer_.size() - position_ < remaining_bytes_ + 2)
                    return ParseStatus::InProgress;
                arguments_.emplace_back(
                    buffer_.substr(position_, remaining_bytes_));
                position_ += remaining_bytes_;
                if (buffer_.compare(position_, 2, "\r\n") != 0)
                    return ParseStatus::Error;
                position_ += 2;
                --remaining_arrays_;
                if (remaining_arrays_ == 0)
                {
                    buffer_.erase(0, position_);
                    position_ = 0;
                    state_ = State::ReadArrayHeader;
                    return ParseStatus::Complete;
                }
                state_ = State::ReadBulkHeader;
                break;
            }
        }
    }
}

bool RespParser::ReadLine(std::string& line)
{
    auto end = buffer_.find("\r\n", position_);

    if (end == std::string::npos)
        return false;

    line = buffer_.substr(position_, end - position_);
    position_ = end + 2;

    return true;
}
