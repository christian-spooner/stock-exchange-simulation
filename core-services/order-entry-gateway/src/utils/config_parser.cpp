#include "utils/config_parser.hpp"

#include <fstream>
#include <stdexcept>

#include "utils/json_util.hpp"

ConfigParser::ConfigParser(const std::string& config_file_path) {
	parse_config_file(config_file_path);
}

std::string ConfigParser::get_rabbitmq_uri() const { return rabbitmq_uri_; }

int ConfigParser::get_server_port() const { return 2400; }

void ConfigParser::parse_config_file(const std::string& config_file_path) {
	std::ifstream config_file_stream(config_file_path);
	if (!config_file_stream.is_open()) {
		throw std::runtime_error("Could not open configuration file: " + config_file_path);
	}

	json config_json;
	config_file_stream >> config_json;

	if (config_json.contains("rabbitmq_uri")) {
		rabbitmq_uri_ = config_json["rabbitmq_uri"].get<std::string>();
	} else {
		throw std::runtime_error("Configuration file is missing 'rabbitmq_uri' field.");
	}
}
