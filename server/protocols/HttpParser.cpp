#include <sstream>
#include <string>
#include <vector>
#include "HttpParser.hpp"

void HttpParser::Parse(const std::string_view data)
{
    buffer_.append(data);
    if (CheckIfHeaderNotComplete())
    {
        parse_status_ = HeaderParseStatus::InProgress;
        return;
    }
    CreateHttpRequest();
    parse_status_ = HeaderParseStatus::Complete;
}

bool HttpParser::CheckIfHeaderNotComplete()
{
    auto header_end = buffer_.find("\r\n\r\n");
    return header_end == std::string::npos;
}

void HttpParser::CreateHttpRequest()
{
    std::size_t cursor = 0;
    auto find_crlf = [&](std::size_t from) -> std::size_t {
        return buffer_.find("\r\n", from);
    };

    std::size_t pos = find_crlf(cursor);
    if (pos == std::string::npos)
        return; // incomplete request line

    std::string line = buffer_.substr(cursor, pos - cursor);
    {
        std::istringstream iss(line);
        iss >> request_.method;
        iss >> request_.path_str;
        iss >> request_.version;
    }

    cursor = pos + 2;

    // Headers
    while (true)
    {
        pos = find_crlf(cursor);
        if (pos == std::string::npos)
            return; // incomplete headers, keep buffer unchanged

        line = buffer_.substr(cursor, pos - cursor);
        // Empty line = end of headers
        if (line.empty())
        {
            cursor = pos + 2;
            break;
        }

        std::size_t separator = line.find(':');
        if (separator != std::string::npos)
        {
            std::string name = line.substr(0, separator);
            std::string value = line.substr(separator + 1);
            // trim one leading space after ':'
            if (!value.empty() && value.front() == ' ')
                value.erase(0, 1);

            // request_.headers[name] = value;
            headers[name] = value;
            // Catch Content-Length only (no body consume)
            if (name == "Content-Length")
            {
                try
                {
                    request_.content_length = static_cast<std::size_t>(std::stoull(value));
                }
                catch (...)
                {
                    request_.content_length = 0; // TODO should it raise an alert?
                }
            }
        }
        cursor = pos + 2;
    }

    buffer_.erase(0, cursor);
    request_.body = {std::make_move_iterator(buffer_.begin()), std::make_move_iterator(buffer_.end())};
    buffer_.clear();
}

void HttpParser::ClearRequest()
{
    request_.method.clear();
    request_.path_str.clear();
    request_.version.clear();
    request_.content_length = 0;
    request_.body.clear();
}
