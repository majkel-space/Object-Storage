#include <iostream>
#include "HttpParser.hpp"

ParseStatus HttpParser::Parse(const std::string_view data)
{
    buffer_.append(data);
    if (CheckIfdataComplete())
    {
        std::cout << "HTTTP Complete msg " << buffer_ << std::endl;
        return ParseStatus::Complete;
    }
    std::cout << "HTTTP Chunk " << data << std::endl;
    return ParseStatus::InProgress;
}

bool HttpParser::CheckIfdataComplete()
{
    if (ending.size() > buffer_.size())
        return false;

    return buffer_.ends_with(ending);
}
