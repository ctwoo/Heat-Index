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

    response = validate_air_temp(query_params, response);
    if (!response.valid) return response;
    response = validate_air_temp_uom(query_params, response);
    if (!response.valid) return response;
    response = validate_relative_humidity(query_params, response);
    if (!response.valid) return response;
    response = validate_dewpoint(query_params, response);
    if (!response.valid) return response;
    response = validate_dewpoint_uom(query_params, response);
    if (!response.valid) return response;

    response = validate_input_values(response);
    if (!response.valid) return response;
    response = validate_dew_rel_hum (response);

    return response;
}

response_t validate_air_temp (const kvp& query_params, const response_t& response) {
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

response_t validate_air_temp_uom(const kvp& query_params, const response_t& response) {
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

response_t validate_relative_humidity (const kvp& query_params, const response_t& response) {
    auto r = response;
    auto it = query_params.find("relative_humidity");
    if (it != query_params.end() && !it->second.empty()) {
	if (numeric(it->second)) {
	    r.input.relative_humidity = atof(it->second.c_str());
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

response_t validate_dewpoint (const kvp& query_params, const response_t& response) {
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

response_t validate_dewpoint_uom(const kvp& query_params, const response_t& response) {
    auto r = response;
    auto it = query_params.find("dew_uom");
    if (it != query_params.end()) {
	string dew_uom = it->second;
	dew_uom = toupper(dew_uom[0]);
	if (dew_uom == "C" || dew_uom == "F") {
	    r.input.dew_uom = dew_uom;
	}
	else {
	    r.valid = false;
	    r.doc["status"] = "error";
	    r.doc["message"] = "Unknown unit of measure provided.";
	    r.doc["expected"] = "One of 'dew_uom=C' or 'dew_uom=F'.";
	    r.doc["actual"] = it->second;
	}

    }
    std::cout << "\ndp uom " << r.input.dew_uom << '\n';
    return r;
}

response_t validate_input_values(const response_t& response) {
    auto r = response;
    auto air_max_bound = 80;
    auto dew_min_bound = 10;
    if (r.input.air_uom == "F") {
        air_max_bound = 80;
    }
    else if (r.input.air_uom == "C") {
        air_max_bound = 26.66667;
    }

    if (r.input.air_temp < air_max_bound) {
	r.valid = false;
	r.doc["status"] = "error";
	r.doc["message"] =
	    "The valid input limits for air temperature is greater than 80 deg Fahrenheit or 26.66667 deg Celsius.";
	return r;
    }
    if (!r.input.relative_humidity=second.empty) {
        if (r.input.relative_humidity < 40.0 || r.input.relative_humidity > 100) {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "The valid input limits for relative humidity is greater than 40 and less than 100.";
        }
    }
    if (r.input.dew_uom == "C") {
        dew_min_bound = -243;
    } else if (r.input.dew_uom == "F") {
        dew_min_bound = -405.4;
    }
    if (r.input.dew_temp < dew_min_bound || r.input.dew_temp > r.input.air_temp) {
        r.valid = false;
        r.doc["status"] = "error";
        r.doc["message"] =
            "The valid input limits for dewpoint temperature are between -243 deg Celsius (or -405.4 deg Fahrenheit) and the input air temperature.";
    } else {
	    r.input.is_dp_set = true;
    }
    else {
	r.input.is_rh_set = true;
    }
    std::cout << "\nuse rh? " << r.input.is_rh_set << '\n';
    std::cout << "\nuse dp? " << r.input.is_dp_set << '\n';
    return r;
}
// Prevents calculation if BOTH relative humidity and dewpoint temperature are input
response_t validate_dew_rel_hum (const response_t& response) {
    auto r = response;
    if (r.input.is_rh_set && r.input.is_dp_set) {
	r.valid = false;
	r.doc["status"] = "error";
	r.doc["message"] = "Requires rh or dew_temp, not both.";
	r.doc["expected"] = "a floating point value rh = (40,100) or dew_temp = [-405.4F , air_temp]";
    }
        else if (!r.input.is_rh_set && !r.input.is_dp_set) {
        r.valid = false;
        r.doc["status"] = "error";
        r.doc["message"] = "Requires rh or dew_temp input in addition to air_temp input.";
        r.doc["expected"] = "a floating point value rh = (40,100) or dew_temp = [-405.4F , air_temp]";
    }
    return r;
}

response_t calculate (const response_t& response) {
    auto r = response;
    auto air_temp = r.input.air_temp;
    auto dewpoint = r.input.dew_temp;
    if (r.input.air_uom == "F") {
        air_temp = cvt_f_c(r.input.air_temp);
    }
    if (r.input.dew_uom == "F") {
        dewpoint = cvt_f_c(r.input.dew_temp);
    }
    if (r.input.is_dp_set == true) {
	r.input.relative_humidity = calculate_relative_humidity(air_temp, dewpoint);
    }
    auto heat_index = calculate_heat_index(air_temp, r.input.relative_humidity);
    r.doc["data"]["heat_index"] = make_json_pair("deg F", heat_index);

    return r;
}


