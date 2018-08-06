#pragma once

#include <map>
#include <string>

// Converts a character to its hex equivalent
char to_hex(const char code);

// Converts a hex character to its char eqivalent
char from_hex(const char ch);

// Transform URL encoded characters into plain text.
std::string decode(const std::string& source);

enum class StatusCode { ok = 200, no_content = 204, bad = 400};

// return a json content-type
std::string json_header (StatusCode code = StatusCode::ok);



