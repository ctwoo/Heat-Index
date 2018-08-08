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
response_t isvalid (const std::map<std::string, std::string>& query_params);

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
double vapor_pressure (double air_temp) {
    return 6.112 * std::exp((17.62 * air_temp)/(243.12 + air_temp));
    }

// Calculate Relative Humidity Percentage
double rh (double air_temp, double dew_temp) {
    return (vapor_pressure(air_temp)/vapor_pressure(dew_temp))*100;
    }

// Calculate Heat Index using the formula from ...
double heat_index(double air_temp , double rh) {
    constexpr rh2 = rh*rh;
    constexpr at2 = air_temp*air_temp;

    return -42.379
           + (2.04901523 * air_temp)
           + (10.14333127 * rh)
           - (0.22475541  * air_temp * rh)
           - (0.00683783  * at2)
           - (0.05481717  * rh2)
           + (0.00122874  * at2 * rh)
           + (0.00085282  * air_temp * rh2)
           - (0.00000199  * at2*rh2);
    }




