#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../calc.h"
#include <doctest.h>
#include <nlohmann/json.hpp>

#include <map>
#include <string>
using std::string;
using nlohmann::json;

SCENARIO( "Validate air temp inputs" ) {
    std::map<string,string> qp;

    WHEN ("Air temp is valid") {
	qp["air_temp"] = "22";
	response_t expected = {true, {"status","success"}};
	response_t actual = expected;
	expected.input.air_temp = 22;
	actual = validate_air_temp(qp, actual);
	THEN ("input struct stores the input air temp") {
	    REQUIRE(actual.input.air_temp == doctest::Approx(expected.input.air_temp));
	}
	AND_THEN ("response struct is flagged as valid") {
	    REQUIRE(actual.valid == expected.valid);
	}
    }

    WHEN ("Air temp is not reasonable for heat index") {
	qp["air_temp"] = "10";
	response_t expected = {true, {"status","success"}};
	response_t actual = expected;
	expected.valid = false;
	actual = validate_air_temp(qp, actual);
	THEN ("response struct is flagged as invalid") {
	    REQUIRE(actual.valid == expected.valid);
	}
    }

}

