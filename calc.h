#pragma once

#include <cmath>
#include <map>
#include <string>
#include <cmath>
#include <nlohmann/json.hpp>

struct input_data_t {
    double air_temp = -99.0;
    double dew_temp = -500.0;
    double rh = 105.0;
};


// A pair of response values from the calculator
struct response_t {
    bool valid = false;
    bool echo_input = false;
    nlohmann::json doc;
    // stores the input air temp required by the model (C)
    input_data_t input;
    response_t() = default;
    response_t(bool v, nlohmann::json d)
        : valid{v}, doc{d}
    {}
    response_t(const response_t&) = default;
    response_t& operator=(const response_t& other) = default;
};

// Heat Index calculator
//
// Heat Index is defined as:
// - how hot the air actually "feels" to the human body
//
// Inputs:
//  - Air temperature (deg F)
//  - Dewpoint Temperature (deg C)
//  OR
//  - Air temperature (deg F)
//  - Relative humidity (%)
//
// Output:
//  - Heat Index (deg F)
//
// Perform all required calculations and
// return the response structure
response_t calculate (const response_t& response);

// validate the query string read in by the program
using kvp = std::map<std::string, std::string>;



// Validates whether or not BOTH dewpoint and relative humidity are input
response_t validate (const kvp& query_params);

response_t validate_air_temp (const kvp&, response_t*);
response_t validate_dewpoint (const kvp&, response_t*);
response_t validate_relative_humidity (const kvp&, response_t*);

// build a json object for a specific UOM and value pair
nlohmann::json make_json_param(const std::string& uom, const double& value);

// determine if a string can be converted to a numeric type
bool numeric(std::string value);

// Specific gas constant for water vapor
constexpr double R = 461.514;

// convert Celsius to Kelvin
constexpr double cvt_c_k(double c) { return c + 273.16; }

// convert Kelvin to Celsius
constexpr double cvt_k_c(double k) { return k - 273.16; }

// convert Celsius to Fahrenheit
constexpr double cvt_c_f(double c) { return (9.0 / 5.0) * c + 32.0; }

// convert Fahrenheit to Celcius
constexpr double cvt_f_c(double f) { return (5.0 / 9.0) * (f - 32.0); }


// Calculate Vapor Pressure
inline double vapor_pressure (double air_temp) {
    return 6.112 * std::exp((17.62 * air_temp)/(243.12 + air_temp));
    }

// Calculate Relative Humidity Percentage
inline double relative_humidity (double air_temp, double dew_temp) {
    return (vapor_pressure(air_temp)/vapor_pressure(dew_temp))*100;
    }

// Calculate Heat Index using the formula from ...
inline double heat_index(double air_temp , double relative_humidity) {

    return -42.379
           + (2.04901523  * air_temp)
           + (10.14333127 * relative_humidity)
           - (0.22475541  * air_temp * relative_humidity)
           - (0.00683783  * air_temp*air_temp)
           - (0.05481717  * relative_humidity * relative_humidity)
           + (0.00122874  * air_temp * air_temp * relative_humidity)
           + (0.00085282  * air_temp * relative_humidity * relative_humidity)
           - (0.00000199  * air_temp * air_temp * relative_humidity * relative_humidity);
    }




