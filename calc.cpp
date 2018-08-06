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

    // validate required temperature input
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
    } else {
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
            // No action needed
        } else if (uom == "C") {
            r.input.air_temp = cvt_c_f(r.input.air_temp);
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

    // validate Dewpoint Temperature
    it = query_params.find("dew_temp");
    if (it != query_params.end() && !it->second.empty()) {
        if (numeric(it->second)) {
            r.input.dew_temp atof(it->second.c_str());
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
            r.doc["actual"] = it->second;}

    // Validate optional dewpoint temperature uom
    it = query_params.find("dew_uom");
    if (it != query_params.end()) {
        string dew_uom = it->second;
        dew_uom = toupper(dew_uom[0]);
        if (dew_uom == "C") {
            //no action required
        }
        else if (dew_uom == "F") {
            r.input.dew_temp = cvt_f_c(r.input.dew_temp);
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
    return r;
}

response_t calculate (const response_t& response) {
    auto r = response;

    if (r.input.dew_temp < -243 || r.input.dew_temp > air_temp) {
        r.doc["status"] = "warn";
        r.doc["message"] =
              "The valid input limits for dewpoint temperature are between -243C and the input air temperature";
    }

    auto ah = abs_humidity (r.input.air_temp);
    if (r.input.ah_uom == "kg/m**3") {
        r.doc["data"]["absolute_humidity"] = make_json_pair("kg/m**3", ah);
       }
    else {
        r.doc["data"]["absolute_humidity"] = make_json_pair("g/m**3", ah*1000.0);
        }
    return r;
}


