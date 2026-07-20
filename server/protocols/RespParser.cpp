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
                    parse_status_ = ParseStatus::InProgress;
                if (line.empty() || line[0] != '*')
                {

                    parse_status_ = ParseStatus::Error;
                    return;
                }

                std::size_t parsed = 0;
                try
                {
                    parsed = std::stoul(line.substr(1));
                }
                catch (...)
                {
                    parse_status_ = ParseStatus::Error;
                    return;
                }

                remaining_arrays_ = parsed;
                request_.arguments.clear();
                ClearRequest();
                if (remaining_arrays_ == 0)
                {
                    // Empty array command is valid RESP framing but likely not a valid request for your app.
                    state_ = State::ReadArrayHeader;
                    parse_status_ = ParseStatus::Error;
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
                    parse_status_ = ParseStatus::InProgress;
                    return;
                }
                if (line.empty() || line[0] != '$')
                {
                    parse_status_ = ParseStatus::Error;
                    return;
                }

                std::size_t parsed = 0;
                try
                {
                    parsed = std::stoul(line.substr(1));
                }
                catch (...)
                {
                    parse_status_ = ParseStatus::Error;
                    return;
                }
                request_.content_length = parsed;
                state_ = State::ReadBulkData;
                break;
            }
            case State::ReadBulkData:
            {
                if (buffer_.size() - position_ < request_.content_length + 2)
                {
                    parse_status_ = ParseStatus::InProgress;
                    return;
                }

                if (request_.method.empty())
                    request_.method = buffer_.substr(position_, request_.content_length);
                else
                    request_.arguments.emplace_back(buffer_.substr(position_, request_.content_length));

                    position_ += request_.content_length;
                if (buffer_.compare(position_, 2, "\r\n") != 0)
                {
                    parse_status_ = ParseStatus::Error;
                    return;
                }

                position_ += 2;
                --remaining_arrays_;
                if (remaining_arrays_ == 0)
                {
                    // Remove only consumed message bytes.
                    buffer_.erase(0, position_);
                    position_ = 0;
                    state_ = State::ReadArrayHeader;
                    std::cout << request_;
                    std::cout << "###########\n";
                    //TODO check if exist to not get at exception
                    request_.path = request_.arguments.at(0);
                    request_.object = request_.arguments.at(1);
                    parse_status_ = ParseStatus::Complete;
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
    request_.path.clear();
    request_.object.clear();
    request_.arguments.clear();
    request_.content_length = 0;
}
