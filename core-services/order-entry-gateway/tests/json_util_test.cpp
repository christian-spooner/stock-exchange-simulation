#include "utils/json_util.hpp"

#include "boost/test/included/unit_test.hpp"

using namespace json_util;

BOOST_AUTO_TEST_CASE(parse_json_string_test) {
	std::string json_str =
		"{\"client_id\": 1, \"security_id\": 10, \"side\": "
		"\"buy\", \"price\": 150.0, \"quantity\": 100}";

	json parsed_json = parse_json_string(json_str);

	BOOST_CHECK(parsed_json["client_id"] == 1);
	BOOST_CHECK(parsed_json["security_id"] == 10);
	BOOST_CHECK(parsed_json["side"] == "buy");
	BOOST_CHECK(parsed_json["price"] == 150.0);
	BOOST_CHECK(parsed_json["quantity"] == 100);
}
