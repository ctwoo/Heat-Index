//
// A simple cgi program
// Calculates Heat Index with input (temperature and dewpoint temperature
// OR temperature and relative humidity (%))
// Relative humidity is what is required, but can be calculated if given
// a dewpoint temperature.
//
// Compile with:
// c++ -std=c++11 -Wall -Wextra -pedantic *.cpp -o cgi
//
// Example output
// {
//   "in" : {
//     "Air Temperature": { "value": 20, "uom": "Fahrenheit"},
//     "Dewpoint Temperature": { "value": 20, "uom": "Fahrenheit"}
//   "out" : {
//     "Heat Index": { "value": 0.017293, "uom": "Fahrenheit"}
//   }
// }
//
//
#include "calc.h"
#include "cgi.h"
#include "util.h"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

using std::string;
using json = nlohmann::json;

string version() {
  return "version 1.0.0";
}

static void usage(const char* name) {
  std::cerr << "Usage: " << name << " [-h|--help] [-v] "
#ifdef HAVE_SETENV
            << " [QUERY_STRING=value]\n";
#else
            << '\n';
#endif
}

static void help(const char* name) {
    usage(name);
    std::cerr << "Options:\n"
        << "  -h or --help         Show this text and exit\n"
        << "  -v or --version      Show program version and exit\n"
#ifdef HAVE_SETENV
        << "  QUERY_STRING=value   Set the QUERY_STRING in GET request mode.\n\n";
#else
        << "\n";
#endif
    exit(0);
}

// process command line arguments,
// faking a CGI request, if needed.
void process_args(int argc, char** argv) {
    for (int i=1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "-h") || !std::strcmp(argv[i], "--help")) {
            help(argv[0]);
        } else if (!std::strcmp(argv[i], "-v") || !std::strcmp(argv[i], "--version")) {
            std::cout << version() << '\n';
            exit(0);
#ifdef HAVE_SETENV
        } else if (std::strncmp(argv[i], "QUERY_STRING", 12) == 0) {
            // for testing, allow setting query string on the command line
            setenv("REQUEST_METHOD", "GET", true);
            std::string qs(argv[i]);
            qs.erase(qs.begin(), qs.begin()+13);
            setenv("QUERY_STRING", qs.c_str(), true);
#endif
        } else {
            std::cerr << "Unknown argument received, " << argv[i] << '\n';
            usage(argv[0]);
            exit(-1);
        }
    }
}

int main(int argc, char** argv) {
    cgi in;

    process_args(argc, argv);
    auto count = in.read();
    if (count == 0) {
        return count;
    }

    // We got some kind of GET request.
    // Validate and respond.
    response_t response;
    response = validate(in.kvp);

    if (response.valid) {
      std::cout << json_header();
      response = calculate(response);
    } else {
      std::cout << json_header(StatusCode::bad);
    }
    std::cout << response.doc.dump(4) << std::endl;
    return count;
}
