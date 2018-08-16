
#include "cgi.h"
#include "util.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <utility>

#include <nlohmann/json.hpp>

using std::string;
using std::size_t;
using json = nlohmann::json;


size_t cgi::read() {
    json j;
    j["expected"] = "REQUEST_METHOD=GET";
    auto method = std::getenv("REQUEST_METHOD");
    j["actual"] = nullptr;
    j["status"] = "error";
    if (method == nullptr) {
        j["message"] = "CGI Error: No REQUEST_METHOD specified.";
        std::cout << json_header(StatusCode::bad) << j.dump(4) << std::endl;
    } else if (strcmp(method,"GET") != 0) {
        j["message"] = "CGI Error: Invalid or unsupported method specified.";
        j["actual"] = method;
        std::cout << json_header(StatusCode::bad) << j.dump(4) << std::endl;
    } else {
        auto qs = std::getenv("QUERY_STRING");
        if (qs == nullptr) {
            j["message"] = "CGI Error: No QUERY_STRING provided.";
            j["expected"] = "QUERY_STRING=air_temp=N (where 'N' is -45 to 60 C)";
            std::cout << json_header(StatusCode::bad) << j.dump(4) << std::endl;
        } else {
            return parse_query_string(string(qs));
        }
    }

    return 0;
}

size_t cgi::parse_query_string(const string& qs) {
    if (qs.empty()) return 0;
    std::istringstream iss(qs);
    string pair;
    string key;
    while (std::getline(iss, pair, '&')) {
        auto decoded = decode(pair);
        auto pos = decoded.find('=');
        if (pos != std::string::npos) {
            key = decoded.substr(0,pos);
            if (key == "air_temp" || key == "air_uom" 
             || key == "relative_humidity"
             || key == "dew_temp"
             || key == "dew_uom") {
                kvp[key] = decoded.substr(pos+1);
            } else {
                json j;
                j["message"] = "CGI Error: Unknown QUERY_STRING key.";
                j["expected"] = {"air_temp", "air_uom", "dew_temp", "dew_uom", "relative_humidity"};
                j["actual"] = key;
                j["status"] = "error";
                std::cout << json_header(StatusCode::bad) << j.dump(4) << std::endl;
                return 0;
            }
        }
    }
    if (kvp.empty()) {
        json j;
        j["message"] = "CGI Error: Malformed QUERY_STRING.";
        j["expected"] = "key/value pair";
        j["actual"] = qs;
        j["status"] = "error";
        std::cout << json_header(StatusCode::bad) << j.dump(4) << std::endl;
    }
    return kvp.size();
}


