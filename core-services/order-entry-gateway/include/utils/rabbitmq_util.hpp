#pragma once

#include <string>

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "utils/json_util.hpp"

namespace rabbitmq_util {

AmqpClient::Channel::ptr_t create_channel(const std::string& rabbitmq_uri);
void send_message(const AmqpClient::Channel::ptr_t& channel, const std::string& exchange,
				  const std::string& routing_key, const json& message_json);

}  // namespace rabbitmq_util
