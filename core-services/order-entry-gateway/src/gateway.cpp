#include "gateway.hpp"

#include <chrono>
#include <iostream>

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "utils/json_util.hpp"
#include "utils/rabbitmq_util.hpp"

Gateway::Gateway(const std::string &rabbitmq_uri)
	: rabbitmq_uri_(rabbitmq_uri), connection_(AmqpClient::Channel::CreateFromUri(rabbitmq_uri)) {
	count_.emplace("MSFT", 0);
	count_.emplace("AAPL", 0);
	count_.emplace("TSLA", 0);
	count_.emplace("JPM", 0);
	count_.emplace("SHEL", 0);

	channel_ = connection_;
	channel_->DeclareExchange("stock_exchange", AmqpClient::Channel::EXCHANGE_TYPE_DIRECT, true);
}

std::string Gateway::process_request(const json &request) {
	if (request["type"] == "limit") {
		double priceInDollars = request["price"];
		int priceInCents = static_cast<int>(priceInDollars * 100);
		int sequence_id = ++count_[request["security_id"]];
		json sequenced_order = request;
		sequenced_order["order_id"] = sequence_id;
		sequenced_order["price"] = priceInCents;

		send_to_matching_engine(sequenced_order);
		send_to_data_feed(sequenced_order);
		json response = {
			{"status", "success"}, {"order_id", sequence_id}, {"message", "order submitted"}};
		return json_util::json_to_string(response);
	} else if (request["type"] == "market") {
		int sequence_id = ++count_[request["security_id"]];
		json sequenced_order = request;
		sequenced_order["order_id"] = sequence_id;
		sequenced_order["price"] = 0;

		send_to_matching_engine(sequenced_order);
		send_to_data_feed(sequenced_order, false);
		json response = {
			{"status", "success"}, {"order_id", sequence_id}, {"message", "order submitted"}};
		return json_util::json_to_string(response);
	} else if (request["type"] == "cancel") {
		send_to_matching_engine(request);
		json response = {{"status", "success"}, {"message", "order cancel submitted"}};
		return json_util::json_to_string(response);
	} else {
		json response = {{"status", "failure"}, {"message", "invalid request type"}};
		return json_util::json_to_string(response);
	}
}

void Gateway::send_to_matching_engine(const json &processed_request) {
	std::string message = json_util::json_to_string(processed_request);

	channel_->BasicPublish("stock_exchange", "order_entry",
						   AmqpClient::BasicMessage::Create(message));
}

void Gateway::send_to_data_feed(json &processed_order, const bool convert_price) {
	if (convert_price) {
		int priceInCents = processed_order["price"];
		double priceInDollars = priceInCents / 100.0;
		processed_order["price"] = priceInDollars;
	}

	json order_confirmation;
	order_confirmation["message_type"] = "report";
	processed_order.erase("type");
	processed_order["event_type"] = "order";
	processed_order["status"] = "confirmed";
	order_confirmation["body"] = processed_order;
	std::string message = json_util::json_to_string(order_confirmation);

	channel_->BasicPublish("stock_exchange", "data_feed",
						   AmqpClient::BasicMessage::Create(message));
}
