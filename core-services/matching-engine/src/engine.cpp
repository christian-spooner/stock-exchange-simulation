#include "engine.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <iostream>

#include "utils/json_util.hpp"
#include "utils/logger.hpp"

#ifdef _DEBUG
int execNum;
int cancelNum;
int priceNum;
#endif

// Public member functions

void Engine::limit(Order& order) {
	auto it = bookMap.find(order.securityId);
	if (it != bookMap.end()) {
		auto& book = it->second;
		if (!book->cross_limit_order(order)) {
			book->queue(order);
		}
	}
}

void Engine::market(Order& order) {
	auto it = bookMap.find(order.securityId);
	if (it != bookMap.end()) {
		auto& book = it->second;
		if (!book->cross_market_order(order)) {
			logger.log(LogLevel::INFO,
					   "Discarded market order, id: " + std::to_string(order.orderId) +
						   ", security id: " + order.securityId +
						   ", quantity: " + std::to_string(order.quantity));
		}
	}
}

void Engine::cancel(const OrderId orderId, const std::string securityId) {
	auto it = bookMap.find(securityId);
	if (it != bookMap.end()) {
		auto order = it->second->cancel(orderId);

		if (order.orderId != 0) {
			send_cancel_report(order);
		}
	}
}

void Engine::execute(const Order& buyOrder, const Order& sellOrder, const Quantity execQuantity) {
	if (execQuantity > 0) {
		Price execPrice;
		if (buyOrder.price <= sellOrder.price) {
			execPrice = buyOrder.price;
		} else {
			execPrice = sellOrder.price;
		}

		if (buyOrder.orderId < sellOrder.orderId) {
			Order execOrder(buyOrder.orderId, buyOrder.securityId, buyOrder.clientId, 0,
							execQuantity, execPrice);
			send_execution_report(execOrder);

			execOrder.orderId = sellOrder.orderId;
			execOrder.clientId = sellOrder.clientId;
			execOrder.side = 1;
			send_execution_report(execOrder);
		} else {
			Order execOrder(sellOrder.orderId, sellOrder.securityId, sellOrder.clientId, 1,
							execQuantity, execPrice);
			send_execution_report(execOrder);

			execOrder.orderId = buyOrder.orderId;
			execOrder.clientId = buyOrder.clientId;
			execOrder.side = 0;
			send_execution_report(execOrder);
		}
	}  // ignore orders with quantity 0
}

void Engine::queue_price_change(const std::string securityId, const Price bid, const Price ask) {
	if (bid >= MIN_PRICE && ask <= MAX_PRICE) {
		double dollarBid = bid / 100.0;
		double dollarAsk = ask / 100.0;
		auto now = std::chrono::system_clock::now();
		std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		std::string timestamp = std::to_string(currentTime);
		send_price_report(securityId, dollarBid, dollarAsk, timestamp);
	}
}

// Private member functions

#ifdef _DEBUG
void Engine::send_execution_report(Order& order) { execNum++; }
#else
void Engine::send_execution_report(Order& order) {
	logger.log(LogLevel::INFO, "Executing trade for order id: " + std::to_string(order.orderId) +
								   ", security id: " + order.securityId);

	double dollarPrice = order.price / 100.0;

	json execReport = {{"order_id", order.orderId},	  {"security_id", order.securityId},
					   {"client_id", order.clientId}, {"side", order.side == BUY ? "buy" : "sell"},
					   {"quantity", order.quantity},  {"price", dollarPrice}};
	std::string message = json_util::json_to_string(execReport);

	this->channel->BasicPublish("stock_exchange", "order_execution",
								AmqpClient::BasicMessage::Create(message));

	json body = {{"event_type", "trade"},		{"status", "confirmed"},
				 {"order_id", order.orderId},	{"security_id", order.securityId},
				 {"client_id", order.clientId}, {"side", order.side == BUY ? "buy" : "sell"},
				 {"quantity", order.quantity},	{"price", dollarPrice}};
	json confirmationReport = {{"message_type", "report"}, {"body", body}};
	message = json_util::json_to_string(confirmationReport);

	this->channel->BasicPublish("stock_exchange", "data_feed",
								AmqpClient::BasicMessage::Create(message));
}
#endif

#ifdef _DEBUG
void Engine::send_cancel_report(Order& order) { cancelNum++; }
#else
void Engine::send_cancel_report(Order& order) {
	logger.log(LogLevel::INFO,
			   "Sending cancel report for order id: " + std::to_string(order.orderId) +
				   ", security id: " + order.securityId);

	double dollarPrice = order.price / 100.0;

	json body = {{"event_type", "order"},		{"status", "denied"},
				 {"order_id", order.orderId},	{"security_id", order.securityId},
				 {"client_id", order.clientId}, {"side", order.side == BUY ? "buy" : "sell"},
				 {"quantity", order.quantity},	{"price", dollarPrice},
				 {"reason", "cancelled"}};
	json cancellationReport = {{"message_type", "report"}, {"body", body}};
	std::string message = json_util::json_to_string(cancellationReport);

	this->channel->BasicPublish("stock_exchange", "data_feed",
								AmqpClient::BasicMessage::Create(message));
}
#endif

#ifdef _DEBUG
void Engine::send_price_report(const std::string securityId, const double bid, const double ask,
							   const std::string timestamp) {
	priceNum++;
}
#else
void Engine::send_price_report(const std::string securityId, const double bid, const double ask,
							   const std::string timestamp) {
	std::ostringstream bidStream;
	bidStream << std::fixed << std::setprecision(2) << bid;
	std::string bidStr = bidStream.str();
	std::ostringstream askStream;
	askStream << std::fixed << std::setprecision(2) << ask;
	std::string askStr = askStream.str();

	logger.log(LogLevel::INFO,
			   "Sending price report: " + securityId + ", " + bidStr + ", " + askStr);

	json body = {{"security_id", securityId}, {"bid", bid}, {"ask", ask}, {"timestamp", timestamp}};
	json priceReport = {{"message_type", "price"}, {"body", body}};
	std::string message = json_util::json_to_string(priceReport);

	this->channel->BasicPublish("stock_exchange", "data_feed",
								AmqpClient::BasicMessage::Create(message));
}
#endif
