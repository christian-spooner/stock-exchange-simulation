#pragma once

#include <cassert>
#include <string>

#include "constants.hpp"

/**
 * Order ID assigned by order entry gateway.
 */
typedef unsigned long OrderId;

/**
 * Buying (bid) or selling (ask).
 * Bid=0, Ask=1
 */
typedef bool Side;

/**
 * Maximum number of shares to be bought or sold.
 */
typedef unsigned long Quantity;

/**
 * Limit price at which an order will be executed
 *
 * Prices are stored as integers in the range 0-65536,
 * where each unit represents a value of 0.01 dollars.
 * For example, the price 123.45 would be represented as
 * the integer 12345.
 */
typedef unsigned short Price;

/**
 * A single limit order as received by the order entry gateway.
 */
struct Order {
	OrderId orderId;
	std::string securityId;
	std::string clientId;
	Side side;
	Quantity quantity;
	Price price;

	Order() = default;
	Order(const OrderId oId, const std::string& sId, const std::string& cId, const Side s,
		  Quantity q, const Price p)
		: orderId(oId), securityId(sId), clientId(cId), side(s), quantity(q), price(p) {}
};

struct LimitEntry;

/**
 * A single limit price.
 */
struct Limit {
	Price price;
	unsigned long volume = 0;

	Limit* parent = nullptr;
	Limit* leftChild = nullptr;
	Limit* rightChild = nullptr;

	LimitEntry* head = nullptr;
	LimitEntry* tail = nullptr;

	Limit() = default;
	Limit(Price p) : price(p) {}

	void increase_volume(const unsigned long qty) { volume += qty; }

	void decrease_volume(const unsigned long qty) {
		assert(volume >= qty);
		volume -= qty;
	}
};

/**
 * A single entry within a limit.
 */
struct LimitEntry {
	Order order;
	LimitEntry* nextEntry = nullptr;
	Limit* parentLimit = nullptr;

	LimitEntry() = default;

	void set_order(const Order& newOrder) {
		order = newOrder;
		parentLimit->increase_volume(newOrder.quantity);
	}

	void cancel_order() {
		parentLimit->decrease_volume(order.quantity);
		order.quantity = 0;
	}
};
