#include "utils/json_util.hpp"

#include <stdexcept>

namespace json_util {

json parse_json_string(const std::string& json_string) {
	try {
		return json::parse(json_string);
	} catch (const json::parse_error& e) {
		throw std::runtime_error("Error parsing JSON string: " + std::string(e.what()));
	}
}

std::string json_to_string(const json& json_obj) { return json_obj.dump(); }

}  // namespace json_util
