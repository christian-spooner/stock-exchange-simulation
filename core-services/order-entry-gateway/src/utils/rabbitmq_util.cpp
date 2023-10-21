#include "utils/rabbitmq_util.hpp"

#include "utils/json_util.hpp"

namespace rabbitmq_util {

AmqpClient::Channel::ptr_t create_channel(const std::string& rabbitmq_uri) {
	return AmqpClient::Channel::Create(rabbitmq_uri);
}

void send_message(const AmqpClient::Channel::ptr_t& channel, const std::string& exchange,
				  const std::string& routing_key, const json& message_json) {
	std::string message_str = json_util::json_to_string(message_json);
	channel->BasicPublish(exchange, routing_key, AmqpClient::BasicMessage::Create(message_str));
}

}  // namespace rabbitmq_util
