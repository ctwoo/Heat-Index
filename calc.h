#pragma once

#include <cmath>
#include <map>
#include <string>
#include <cmath>
#include <nlohmann/json.hpp>

struct input_data_t {
    double air_temp = -99.0;
    double dew_temp = -500.0;
    double rh = 100;
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

//Calculate Saturation Vapor Pressure
/*double svp (double air_temp, double dew_temp) {
    return 6.112 * std::exp((17.62 * dew_temp)/(243.12 + dew_temp));
    }

// Calculate Vapor Pressure
double vp (double air_temp, double dew_temp) {
    return 6.112 * std::exp((17.62 * air_temp)/(243.12 + air_temp));
    }

// Calculate Relative Humidity Percentage
double rh (double vp, double svp) {
    return (vp/svp)*100;
    }

// Calculate Heat Index
double h_i(double air_temp , double rh) {
    return (-42.379 + (2.04901523 * air_temp) + (10.14333127 * rh) - (0.22475541* air_temp * rh)-(0.00683783 * pow(air_temp , 2.0)) - (0.05481717 * pow(rh, 2.0)) + (0.00122874 * pow(air_temp, 2.0) * rh) + (0.00085282* air_temp
            * pow(rh , 2.0))- (0.00000199*pow(air_temp,2.0)*pow(rh, 2.0)));
            }
*/



