#include <string>
#include <vector>

namespace helpers
{
    const std::string http_msg =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: TestClient\r\n"
        "Accept: */*\r\n"
        "\r\n";
    const std::string resp_msg = "+OK\r\n";
    const std::string fail_http_msg =
        "GETHTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: TestClient\r\n"
        "Accept: */*\r\n"
        "\r\n";
    const std::string empty_msg = "";
    const std::string http_msg_begin = "GET / HTTP/1.1\r\n";
    const std::string http_msg_end = "\r\n\r\n";
    const std::vector<std::string> resp_msg_chunks {
        "*2\r\n",
        "$3\r\n",
        "GET\r\n",
        "$5\r\n",
        "hello\r\n"
    };
    std::string resp_get_msg = "GET";
    std::string resp_get_incorrect_msg = "GETT";
}
