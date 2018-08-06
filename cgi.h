#pragma once

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

struct cgi {
    std::map<std::string, std::string> kvp;
    cgi() = default;
    ~cgi() = default;
    explicit cgi(const std::string& name, 
                 const std::string& value)
        {
            kvp.emplace(name, value); 
        }

    cgi (const cgi& other) = default;
    cgi& operator=(const cgi& other) = default;
    std::size_t read();
    std::size_t parse_query_string(const std::string& qs);

};

inline bool operator==(const cgi& x, const cgi& y) {
    return x.kvp == y.kvp;
}
inline bool operator!=(const cgi& x, const cgi& y) {
    return !(x == y);
}
inline bool operator<(const cgi& x, const cgi& y) { 
    return x.kvp < y.kvp;
}
inline bool operator>(const cgi& x, const cgi& y) {
    return y < x;
}
inline bool operator<=(const cgi& x, const cgi& y) {
    return !(y < x);
}
inline bool operator>=(const cgi& x, const cgi& y) {
    return !(x < y);
} 

inline std::ostream& operator<<(std::ostream& os, const cgi& rhs) {
    for (const auto& pair: rhs.kvp) {
        os << pair.first << " = " << pair.second << '\n';
    }
    return os;
}

std::string jsonify (const std::string& msg);
std::string jsonify (const std::map<std::string, std::string>& kvp);
