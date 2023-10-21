#pragma once

#include <string>
#include <unordered_map>

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "utils/json_util.hpp"

class Gateway {
   public:
	explicit Gateway(const std::string &rabbitmq_uri);
	std::string process_request(const json &order);

   private:
	void send_to_matching_engine(const json &sequenced_order);
	void send_to_data_feed(json &sequenced_order, const bool convert_price = true);

	std::string rabbitmq_uri_;
	AmqpClient::Channel::ptr_t connection_;
	AmqpClient::Channel::ptr_t channel_;
	std::unordered_map<std::string, int> count_;
};
