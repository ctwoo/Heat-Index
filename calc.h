#pragma once

#include <cmath>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

struct input_data_t {
    double air_temp = -99.0;
    double dew_temp = -500.0;
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

// Absolute Humidity calculator
//
// Absolute Humidity is defined as:
// - the mass of water vapor contained in a unit volume of air.
//
// Inputs:
//  - Air temperature (deg C)
//
// Output:
//  - Absolute humidity (kg / m**3)
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

//Calculate Saturation Vapor Pressure
double svp (double air_temp) {
    return 6.11*10**((7.5*air_temp)/(237.7+air_temp));
    }

// Calculate Vapor Pressure
double vp (double dew_temp) {
    return 6.11*10**((7.5*dew_temp)/(237.7+dew_temp);
    }

// Calculate Relative Humidity Percentage
double rh () {
    return (vp/svp)*100;
    }


