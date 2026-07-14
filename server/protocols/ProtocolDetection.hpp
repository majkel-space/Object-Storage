#include <string_view>

enum class Protocol
{
    Unknown,
    Http,
    Resp
};

Protocol DetectProtocol(const std::string_view msg)
{
    if (msg.empty())
        return Protocol::Unknown;

    switch (msg.front())
    {
    case '*':
    case '$':
    case '+':
    case '-':
    case ':':
        return Protocol::Resp;
    }

    if (msg.starts_with("GET ") or
        msg.starts_with("PUT ") or
        msg.starts_with("DELETE ") or
        msg.starts_with("HEAD "))
            return Protocol::Http;

    return Protocol::Unknown;
}
