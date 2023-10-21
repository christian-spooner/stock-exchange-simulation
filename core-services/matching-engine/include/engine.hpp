#pragma once

#include <memory>
#include <unordered_map>

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "book.hpp"
#include "constants.hpp"
#include "data_types.hpp"

#ifdef _DEBUG
extern int execNum;
extern int cancelNum;
extern int priceNum;
#endif

class Book;

class Engine {
   public:
	Engine() {
		bookMap.emplace("MSFT", std::make_unique<Book>(*this, "MSFT"));
		bookMap.emplace("AAPL", std::make_unique<Book>(*this, "AAPL"));
		bookMap.emplace("GOOG", std::make_unique<Book>(*this, "GOOG"));
		bookMap.emplace("AMZN", std::make_unique<Book>(*this, "AMZN"));
		bookMap.emplace("FB", std::make_unique<Book>(*this, "FB"));

		channel = AmqpClient::Channel::CreateFromUri("amqp://guest:guest@rabbitmq:5672/%2F");
		channel->DeclareExchange("stock_exchange", AmqpClient::Channel::EXCHANGE_TYPE_DIRECT, true);
	}

	~Engine() {
		bookMap.clear();
		channel.reset();
	}

	/**
	 * Add a limit order to the book.
	 * @param order The limit order to add to the book.
	 */
	void limit(Order& order);

	/**
	 * Cross a market order with the book.
	 * @param order The market order to cross with the book.
	 */
	void market(Order& order);

	/**
	 * Cancel an order in the book.
	 * @param orderId The id of the order to be cancelled.
	 */
	void cancel(const OrderId orderId, const std::string securityId);

	/**
	 * Execute an order in the book.
	 * @param order The order to be executed.
	 */
	void execute(const Order& buyOrder, const Order& sellOrder, const Quantity execQuantity);

	/**
	 * Queue a change in the best bid / ask for a given security
	 * @param security_id The id of the security
	 * @param bid The current best bid
	 * @param ask The current best ask
	 */
	void queue_price_change(const std::string securityId, const Price bid, const Price ask);

   private:
	std::unordered_map<std::string, std::unique_ptr<Book>> bookMap;
	AmqpClient::Channel::ptr_t channel;
	void send_execution_report(Order& order);
	void send_cancel_report(Order& order);
	void send_price_report(const std::string securityId, const double bid, const double ask,
						   const std::string timestamp);
};
