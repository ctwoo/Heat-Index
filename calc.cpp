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

response_t isvalid (const std::map<std::string, std::string>& query_params) {
    response_t r = {true, {"status","success"}};

    // Validate required temperature input
    auto it = query_params.find("air_temp");
    if (it != query_params.end() && !it->second.empty()) {
        if(numeric(it->second)) {
            r.input.air_temp = atof(it->second.c_str());
        } else {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "Non-numeric value provided for air_temp.";
            r.doc["expected"] = "a floating point value";
            r.doc["actual"] = it->second;
        }
    } else if (it->second.empty()) {
        r.valid = false;
        r.doc["status"] = "error";
        r.doc["message"] = "No value provided for air_temp input parameter.";
        r.doc["expected"] = "a floating point value";
        r.doc["actual"] = it->second;
    }   /*else if () {
        r.valid = false;
        r.doc["status"] = "error";
        r.doc["message"] = "No value provided for air_temp input parameter.";
        r.doc["expected"] = "a floating point value";
        r.doc["actual"] = it->second;
    } */else {
        r.valid = false;
        r.doc["status"] = "error";
        r.doc["message"] = "Required input parameter not specified.";
        r.doc["expected"] = "air_temp";
        r.doc["actual"] = nullptr;
        return r;
    }

    // Validate Optional Air Temperature UOM
    it = query_params.find("air_uom");
    if (it != query_params.end()) {
        string uom = it->second;
        uom = toupper(uom[0]);
        if (uom == "F") {
            r.input.air_temp = cvt_f_c(r.input.air_temp);
        } else if (uom == "C") {
            // No action needed
        }
          else {
            r.valid = false;
            r.doc["stats"] = "error";
            r.doc["message"] = "Unknown unit of measure provided.";
            r.doc["expected"] = "One of 'uom=C' or 'uom=F'.";
            r.doc["actual"] = it->second;
            return r;
          }
    }

    // Validate Dewpoint Temperature and calculate Relative Humidity
    it = query_params.find("dew_temp");
    if (it != query_params.end() && !it->second.empty()) {
        if (numeric(it->second)) {
            r.input.dew_temp = atof(it->second.c_str());
        } else {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "Non-numeric value provided for dew_temp.";
            r.doc["expected"] = "a floating point value [-405.4 F, air_temp]";
            r.doc["actual"] = it->second;
        }
    }   else if (it->second.empty()) {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "No value provided for dew_temp input parameter.";
            r.doc["expected"] = "a floating point value [-405.4 F, air_temp]";
            r.doc["actual"] = it->second;
        }

    // Validate optional Dewpoint temperature UOM
    it = query_params.find("dew_uom");
    if (it != query_params.end()) {
        string dew_uom = it->second;
        dew_uom = toupper(dew_uom[0]);
        if (dew_uom == "C") {
           // double rh = rh(r.input.air_temp, r.input.dew_temp);
        }
        else if (dew_uom == "F") {
            r.input.dew_temp = cvt_f_c(r.input.dew_temp);
            //double rh = rh(r.input.dew_temp);
        }
        else {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "Unknown unit of measure provided.";
            r.doc["expected"] = "One of 'dew_uom=C' or 'dew_uom=F'.";
            r.doc["actual"] = it->second;
            return r;
        }

    }
    // Validate Relative Humidity Input
    it = query_params.find("rh");
    if (it != query_params.end() && !it->second.empty()) {
        if (numeric(it->second)) {
            r.input.rh = atof(it->second.c_str());
        } else {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "Non-numeric value provided for rh.";
            r.doc["expected"] = "a floating point value (0,100)";
            r.doc["actual"] = it->second;
        }
    }   else if (it->second.empty()) {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "No value provided for rh input parameter.";
            r.doc["expected"] = "a floating point value (0,100)";
            r.doc["actual"] = it->second;
        }

    // Prevents calculation if BOTH relative humidity and dewpoint temperature are input.

    it = query_params.find("rh");
    auto it2 = query_params.find("dew_temp");
    if (it != query_params.end() && !it->second.empty()) {
        if (it2 != query_params.end() && !it2->second.empty()) {
            r.valid = false;
            r.doc["status"] = "error";
            r.doc["message"] = "Requires rh or dew_temp, not both.";
            r.doc["expected"] = "a floating point value rh = (0,100) or dew_temp = [-405.4F , air_temp]";
            r.doc["actual"] = it->second, it2->second;
        }
    }
    return r;
}

// Heat Index Calculation

response_t calculate (const response_t& response) {
    auto r = response;

    if (r.input.dew_temp < -243 || r.input.dew_temp > r.input.air_temp) {
        r.doc["status"] = "warn";
        r.doc["message"] =
              "The valid input limits for dewpoint temperature are between -243C and the input air temperature";
    }

// Determine which path to take
/*    path (const std::map<std::string, std::string>& query_params) {
    auto it = query_params.find("rh");
    auto it2 = query_params.find("dew_temp");
    if (!it->second.empty() && it2 ->second.empty()) {
        r.input.air_temp = cvt_c_f(r.input.air_temp);
        auto heat_index = calc(r.input.air_temp,r.input.rh);
    }
    else if(!it2->second.empty() && it->second.empty()) {
        auto rh_conversion = rh(double r.input.air_temp, double r.input.air_temp);
        r.input.air_temp = cvt_c_f(r.input.air_temp);
        auto heat_index = calc(r.input.air_temp, rh_conversion);
    }
    }

r.doc["data"]["heat_index"] = make_json_pair("F", heat_index);

    return r;*/
}


