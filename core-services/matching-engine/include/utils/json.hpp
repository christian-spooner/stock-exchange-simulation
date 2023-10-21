#pragma once

#include <string>

#include "json.hpp"

using json = nlohmann::json;

namespace json_util {

json parse_json_string(const std::string& json_string);
std::string json_to_string(const json& json_obj);

}  // namespace json_util
