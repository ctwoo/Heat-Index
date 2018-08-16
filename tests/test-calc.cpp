#undef DOCTEST_CONFIG_POSIX_SIGNALS

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
	validate_air_temp(qp, &actual);
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
	validate_air_temp(qp, &actual);
	THEN ("response struct is flagged as invalid") {
	    REQUIRE(actual.valid == expected.valid);
	}
    }

//    WHEN ("Air temp is not a number") {
//	qp["air_temp"] = "foo";
//	response_t expected = {false, {{"status","error"}};
//	response_t actual = expected;
//	expected.input.air_temp = -99;
//	validate_air_temp(qp, &actual);
//	THEN ("input struct doesn't modify the input air temp") {
//	    REQUIRE(actual.input.air_temp == doctest::Approx(expected.input.air_temp));
//	}
//	AND_THEN ("response struct is flagged as invalid") {
//	    REQUIRE(actual.valid == expected.valid);
//	}
//    }

}

