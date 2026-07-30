#include "RespParser.hpp"

void RespParser::Parse(const std::string_view data)
{
    if (!data.empty())
        buffer_.append(data);
    while (true)
    {
        switch (state_)
        {
            case State::ReadArrayHeader:
            {
                std::string line;
                if (!ReadLine(line))
                    parse_status_ = HeaderParseStatus::InProgress;
                if (line.empty() || line[0] != '*')
                {
                    parse_status_ = HeaderParseStatus::Error;
                    return;
                }

                std::size_t parsed = 0;
                try
                {
                    parsed = std::stoul(line.substr(1));
                }
                catch (...)
                {
                    parse_status_ = HeaderParseStatus::Error;
                    return;
                }

                remaining_arrays_ = parsed;
                ClearRequest();
                if (remaining_arrays_ == 0)
                {
                    // Empty array command is valid RESP framing but likely not a valid request for your app.
                    state_ = State::ReadArrayHeader;
                    parse_status_ = HeaderParseStatus::Error;
                    return;
                }

                state_ = State::ReadBulkHeader;
                break;
            }
            case State::ReadBulkHeader:
            {
                std::string line;
                if (!ReadLine(line))
                {
                    parse_status_ = HeaderParseStatus::InProgress;
                    return;
                }
                if (line.empty() || line[0] != '$')
                {
                    parse_status_ = HeaderParseStatus::Error;
                    return;
                }

                std::size_t parsed = 0;
                try
                {
                    parsed = std::stoul(line.substr(1));
                }
                catch (...)
                {
                    parse_status_ = HeaderParseStatus::Error;
                    return;
                }
                request_.content_length = parsed;

                if (remaining_arrays_ == 1)
                {
                    buffer_.erase(0, position_);
                    position_ = 0;
                    // Take only up to content_length bytes — strips \r\n framing
                    size_t available = std::min(buffer_.size(), request_.content_length);
                    request_.body.assign(buffer_.begin(), buffer_.begin() + available);
                    buffer_.erase(0, available);
                    // Discard trailing \r\n if already in buffer
                    if (buffer_.size() >= 2 && buffer_[0] == '\r' && buffer_[1] == '\n')
                        buffer_.erase(0, 2);
                    parse_status_ = HeaderParseStatus::Complete;
                    return;
                }
                state_ = State::ReadBulkData;
                break;
            }
            case State::ReadBulkData:
            {
                if (buffer_.size() - position_ < request_.content_length + 2)
                {
                    parse_status_ = HeaderParseStatus::InProgress;
                    return;
                }
                if (request_.method.empty())
                    request_.method = buffer_.substr(position_, request_.content_length);
                else if (request_.path_str.empty())
                    request_.path_str = buffer_.substr(position_, request_.content_length);
                else{
                    std::string_view body_data(buffer_.data() + position_, request_.content_length);
                    request_.body.insert(request_.body.end(), body_data.begin(), body_data.end());
                }
                    // request_.arguments.emplace_back(buffer_.substr(position_, request_.content_length));
                position_ += request_.content_length;
                if (buffer_.compare(position_, 2, "\r\n") != 0)
                {
                    parse_status_ = HeaderParseStatus::Error;
                    return;
                }
                position_ += 2;
                --remaining_arrays_;
                if (remaining_arrays_ == 0)
                {
                    state_ = State::ReadArrayHeader;
                    parse_status_ = HeaderParseStatus::Complete;
                    return;
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

void RespParser::ClearRequest()
{
    request_.method.clear();
    request_.path_str.clear();
    request_.content_length = 0;
}
