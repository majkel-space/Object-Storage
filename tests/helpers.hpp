#include <string>
#include <vector>

namespace helpers
{

const std::string test_output_path = "../test/test_output/";

const std::string fail_http_msg =
    "GETHTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: TestClient\r\n"
    "Accept: */*\r\n"
    "\r\n";
const std::string empty_msg = "";
const std::string resp_msg =
    "*2\r\n"
    "$3\r\n"
    "GET\r\n"
    "$5\r\n"
    "hello\r\n";

const std::string http_list_msg = "GET / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: TestClient\r\n"
    "Accept: */*\r\n"
    "Content-Length: 17\r\n"
    "\r\n+++"
    "Hello world"
    "!!!";
const std::string http_get_msg = "GET aaa HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: TestClient\r\n"
    "Accept: */*\r\n"
    "Content-Length: 17\r\n"
    "\r\n+++"
    "Hello world"
    "!!!";
const std::string http_put_msg = "PUT aaa HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: TestClient\r\n"
    "Accept: */*\r\n"
    "Content-Length: 17\r\n"
    "\r\n+++"
    "Hello world"
    "!!!";

const std::vector<std::string> http_put_msg_chunks {
    {"PUT aaa_http_put HT"},
    {"TP/1.1\r\n"},
    {"Host: localhost\r\n"},
    {"User-Agent: TestClient\r\n"},
    {"Accept: */*\r\n"},
    {"Content-Length: 16\r\n"},
    {"\r\n+++"},
    {"Hello http"},
    {"!!!"}
};
const std::vector<std::string> resp_set_msg_chunks {
    {"*3\r\n"},
    {"$3\r\n"},
    {"SET\r\n"},
    {"$12\r\n"},
    {"aaa_resp_obj\r\n"},
    {"$12\r\n"},
    {"Hello resp\r\n"},
};

const std::vector<std::string> resp_setnx_msg_chunks {
    {"*3\r\n"},
    {"$5\r\n"},
    {"SETNX\r\n"},
    {"$12\r\n"},
    {"aaa_resp_obj\r\n"},
    {"$12\r\n"},
    {"Hello resp\r\n"},
};

const std::vector<std::string> http_get_http_obj_chunks{
    {"GE"},
    {"T aaa_htt"},
    {"p_put HTTP/1.1\r\n\r\n"}
};
const std::vector<std::string> http_get_resp_obj_chunks{
    {"GE"},
    {"T aaa_resp_obj"},
    {" HTTP/1.1\r\n\r\n"}
};
const std::vector<std::string> resp_get_http_obj_chunks{
    {"*2\r\n"},
    {"$3\r\n"},
    {"GET\r\n"},
    {"$12\r\n"},
    {"aaa_http_put\r\n"},
};
const std::vector<std::string> resp_get_resp_obj_chunks{
    {"*2\r\n"},
    {"$3\r\n"},
    {"GET\r\n"},
    {"$12\r\n"},
    {"aaa_resp_obj\r\n"},
};

} //namespace helpers
