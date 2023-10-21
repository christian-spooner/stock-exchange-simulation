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

bool validate_request_json(const json& request_json) {
	if (!request_json.contains("type") || !request_json["type"].is_string()) {
		return false;
	}

	if (request_json["type"] == "limit") {
		if (!request_json.contains("security_id") || !request_json["security_id"].is_string()) {
			return false;
		} else if (!request_json.contains("client_id") || !request_json["client_id"].is_string()) {
			return false;
		} else if (!request_json.contains("side") || !request_json["side"].is_string()) {
			return false;
		} else if (!request_json.contains("price") || !request_json["price"].is_number()) {
			return false;
		} else if (!request_json.contains("quantity") ||
				   !request_json["quantity"].is_number_integer()) {
			return false;
		}
	} else if (request_json["type"] == "market") {
		if (!request_json.contains("security_id") || !request_json["security_id"].is_string()) {
			return false;
		} else if (!request_json.contains("client_id") || !request_json["client_id"].is_string()) {
			return false;
		} else if (!request_json.contains("side") || !request_json["side"].is_string()) {
			return false;
		} else if (!request_json.contains("quantity") ||
				   !request_json["quantity"].is_number_integer()) {
			return false;
		}
	} else if (request_json["type"] == "cancel") {
		if (!request_json.contains("order_id") || !request_json["order_id"].is_number_integer()) {
			return false;
		}
	} else {
		return false;
	}

	return true;
}

}  // namespace json_util
