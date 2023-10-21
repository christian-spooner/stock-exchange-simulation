#pragma once

#include "constants.hpp"
#include "data_types.hpp"
#include "engine.hpp"
#include "limit_buffer.hpp"
#include "limit_entry_buffer.hpp"
#include "limit_tree.hpp"

class Engine;

class Book {
   public:
	Book(Engine& engine, const std::string& securityId)
		: parentEngine(engine),
		  bookSecurityId(securityId),
		  limits(),
		  entries(securityId),
		  bidTree(),
		  askTree() {
		auto minLimit = limits.get(0);
		auto maxLimit = limits.get(MAX_PRICE + 1);
		bidMax = minLimit;
		askMin = maxLimit;
		bidTree.insert(bidMax);
		askTree.insert(askMin);
	}

	~Book() {}

	bool cross_limit_order(Order& order);
	bool cross_market_order(Order& order);
	void queue(Order& order);
	Order cancel(const OrderId orderId);

	Limit* get_bid_max() const { return bidMax; }
	Limit* get_ask_min() const { return askMin; }

   private:
	Engine& parentEngine;
	std::string bookSecurityId;
	LimitBuffer limits;
	LimitEntryBuffer entries;
	LimitTree bidTree;
	LimitTree askTree;
	Limit* bidMax;
	Limit* askMin;

	bool match_at_limit(Engine& parentEngine, Order& order, Limit& limit);
	void update_limit(const bool limitEntryFilled, Limit& limit, LimitEntry* limitEntry);
	void update_limit_entries(Limit* limit, LimitEntry* newEntry, const Order& order);
	void refresh_bid_max();
	void refresh_ask_min();
};
