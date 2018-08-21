#include "calc.h"

#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include <nlohmann/json.hpp>

using std::string;
using json = nlohmann::json;

json make_json_pair(const string& uom, const double& value) {
    return {{"uom", uom}, {"value", value}};
}

bool numeric(string value) {
    auto ss = std::stringstream(value);
    double tmp;
    ss >> tmp;
    return !(!ss || ss.rdbuf()->in_avail() > 0);
}
response_t validate(const kvp& query_params)
{
    response_t response {true, {"status","success"}};
    // What should happen is this path happens if relative humidity input is empty:
    // Validates the following:
    // 1. dewpoint query string
    // 2. optional dewpoint uom
    // 3. air temperature value
    // 4. dewpoint temperature value

    response = read_air_temp(query_params, response);
    if (!response.valid) return response;
    response = read_air_temp_uom(query_params, response);
    if (!response.valid) return response;
    auto temp = response.input.air_temp;
    auto min_t = response.input.min_temp;
    auto max_t = response.input.max_temp;
    if (response.input.air_uom == "C") {
	min_t = cvt_f_c(min_t);
	max_t = cvt_f_c(max_t);
    }
    if (!is_temp_valid(temp, min_t, max_t)) {
	response.valid = false;
	response.doc["status"] = "error";
	std::string msg = "The valid input limits for air temperature are between ";
	msg += min_t;
	msg += " and ";
	msg += max_t;
	msg += " ";
	msg += response.input.air_uom;
	response.doc["message"] = msg;
    }

    if (key_provided("relative_humidity", query_params) &&
        key_provided("dew_temp", query_params)) {
	response.valid = false;
	response.doc["status"] = "error";
	response.doc["message"] = "Requires exactly one of rh or dewpoint.";
	return response;
    }
    if (quantity_provided("relative_humidity", query_params)) {
	response = read_relative_humidity(query_params, response);
	if (!response.valid) return response;
	if (response.input.relative_humidity < 40.0 || response.input.relative_humidity > 100) {
	    response.valid = false;
	    response.doc["status"] = "error";
	    response.doc["message"] = "The valid input limits for relative humidity is greater than 40 and less than 100.";
	    return response;
	} else {
	    response.input.is_rh_set = true;
	}

    } else if (quantity_provided("dew_temp", query_params)) {
	response = read_dewpoint(query_params, response);
	if (!response.valid) return response;
	auto dewpoint = response.input.dew_temp;
	if (is_temp_valid(dewpoint, min_t, temp)) {
	    response.input.is_dp_set = true;
	} else {
	    response.valid = false;
	    response.doc["status"] = "error";
	    std::string msg = "The valid input limits for dewpoint temperature are between ";
	    msg += min_t;
	    msg += " and ";
	    msg += temp;
	    msg += " ";
	    msg += response.input.air_uom;
	    response.doc["message"] = msg;
	    return response;
	}
    } else {
        response.valid = false;
        response.doc["status"] = "error";
        response.doc["message"] = "Missing one of either rh or dewpoint input.";
    }

    return response;
}

bool key_provided(const std::string& quantity, const kvp& query_params) {
    auto it = query_params.find(quantity);
    return it != query_params.end();
}
bool quantity_provided(const std::string& quantity, const kvp& query_params) {
    auto it = query_params.find(quantity);
    return it != query_params.end() && !it->second.empty() && numeric(it->second);
}

bool is_temp_valid(double value, const double min_temp, const double max_temp) {
    return value > min_temp && value < max_temp;
}

response_t read_air_temp (const kvp& query_params, const response_t& response) {
    auto r = response;
    std::cout << "qp size: " << query_params.size() << std::endl;
    auto it = query_params.find("air_temp");
    std::cout << "key: " << it->first
	<< "\tvalue: " << it->second << std::endl;
    if (it != query_params.end() && !it->second.empty()) {
	if(numeric(it->second)) {
	    r.input.air_temp = atof(it->second.c_str());
	}   else {
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "Non-numeric value provided for air_temp.";
	    r.doc["expected"] = "a floating point value";
	    r.doc["actual"] = it->second;
	}
    } else {
	if (it->second.empty()) {
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "No value provided for air_temp input parameter.";
	    r.doc["expected"] = "a floating point value >80 deg Fahrenheit";
	    r.doc["actual"] = it->second;
	}   else {
	    r.valid = false;

	    r.doc["status"] = "error";
	    r.doc["message"] = "Required input parameter not specified.";
	    r.doc["expected"] = "air_temp";
	    r.doc["actual"] = nullptr;
	}
    }
    std::cout << "\nair temp " << r.input.air_temp << '\n';

    return r;
}

response_t read_air_temp_uom(const kvp& query_params, const response_t& response) {
    auto r = response;
    auto it = query_params.find("air_uom");
    if (it != query_params.end()) {
	string uom = it->second;
	uom = toupper(uom[0]);
	if (uom == "F" || uom == "C") {
	    r.input.air_uom = uom;
	}
	else {
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "Unknown unit of measure provided.";
	    r.doc["expected"] = "One of 'uom=C' or 'uom=F'.";
	    r.doc["actual"] = it->second;
	}
    }
    std::cout << "\nair uom " << r.input.air_uom << '\n';
    return r;
}

response_t read_relative_humidity (const kvp& query_params, const response_t& response) {
    auto r = response;
    auto it = query_params.find("relative_humidity");
    if (it != query_params.end() && !it->second.empty()) {
	if (numeric(it->second)) {
	    r.input.relative_humidity = atof(it->second.c_str());
	    /*r.input.mark_rh = true;*/
	}   else if (it->second.empty()) {
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "No value provided for relative_humidity input parameter.";
	    r.doc["expected"] = "a floating point value (0,100)";
	    r.doc["actual"] = it->second;
	}
	else {
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "Non-numeric value provided for relative_humidity.";
	    r.doc["expected"] = "a floating point value (0,100)";
	    r.doc["actual"] = it->second;
	}
    }
    std::cout << "\nrh " << r.input.relative_humidity << '\n';
    return r;
}

response_t read_dewpoint (const kvp& query_params, const response_t& response) {
    auto r = response;
    auto it = query_params.find("dew_temp");
    if (it != query_params.end() && !it->second.empty()) {
	if (numeric(it->second)) {
	    r.input.dew_temp = atof(it->second.c_str());
	}   else if (it->second.empty()){
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "No value provided for dew_temp input parameter.";
	    r.doc["expected"] = "a floating point value [-405.4 F, air_temp]";
	    r.doc["actual"] = it->second;
	}
	else {
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "Non-numeric value provided for dew_temp.";
	    r.doc["expected"] = "a floating point value [-405.4 F, air_temp]";
	    r.doc["actual"] = it->second;
	}
    }
    std::cout << "\ndewpoint " << r.input.dew_temp << '\n';
    return r;
}


bool both_rh_dp_set (const response_t& r) {
    return r.input.is_rh_set && r.input.is_dp_set;
}
bool neither_rh_dp_set (const response_t& r) {
    return !r.input.is_rh_set && !r.input.is_dp_set;
}




response_t calculate (const response_t& response) {
    auto r = response;
    auto air_temp_F = r.input.air_temp;
    auto air_temp_C = r.input.air_temp;
    auto dewpoint_C = r.input.dew_temp;
    if (r.input.air_uom == "F") {
	air_temp_C = cvt_f_c(r.input.air_temp);
	dewpoint_C = cvt_f_c(r.input.dew_temp);
    } else {
	air_temp_F = cvt_c_f(r.input.air_temp);
    }
    if (r.input.is_dp_set) {
	r.input.relative_humidity = calculate_relative_humidity(air_temp_C, dewpoint_C);
    }
    auto heat_index = calculate_heat_index(air_temp_F, r.input.relative_humidity);
    r.doc["data"]["heat_index"] = make_json_pair("deg F", heat_index);

    return r;
}


