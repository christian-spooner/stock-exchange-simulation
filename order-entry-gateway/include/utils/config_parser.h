#pragma once

#include <string>

class ConfigParser {
   public:
	explicit ConfigParser(const std::string& config_file_path);
	std::string get_rabbitmq_uri() const;
	int get_server_port() const;

   private:
	void parse_config_file(const std::string& config_file_path);
	std::string rabbitmq_uri_;
};
