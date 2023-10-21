#include "book.hpp"

#include <algorithm>
#include <cassert>

#define FILLED 1

// Public member functions

void Book::queue(Order& order) {
	Limit* limit = limits.get(order.price);
	LimitEntry* newEntry = entries.new_entry(order, limit);
	update_limit_entries(limit, newEntry, order);

	if ((order.side == BUY && bidMax->price < order.price) ||
		(order.side != BUY && askMin->price > order.price)) {  // update bidMax/askMin if necessary
		if (order.side == BUY) {
			bidMax = limit;
		} else {
			askMin = limit;
		}
		parentEngine.queue_price_change(bookSecurityId, bidMax->price, askMin->price);
	}
}

bool Book::cross_limit_order(Order& order) {
	if (order.side == BUY) {  // cross with asks
		bool askMinChanged = false;

		while (order.price >= askMin->price) {
			bool isOrderFilled = match_at_limit(parentEngine, order, *askMin);
			bool isLimitFilled = askMin->volume == 0;

			if (isLimitFilled) {
				askTree.remove(askMin);
				refresh_ask_min();
				askMinChanged = true;
			}

			if (isOrderFilled) {
				if (askMinChanged) {
					parentEngine.queue_price_change(bookSecurityId, bidMax->price, askMin->price);
				}
				return FILLED;
			}
		}
	} else {  // cross with bids
		bool bidMaxChanged = false;

		while (order.price <= bidMax->price) {
			bool isOrderFilled = match_at_limit(parentEngine, order, *bidMax);
			bool isLimitFilled = bidMax->volume == 0;

			if (isLimitFilled) {
				bidTree.remove(bidMax);
				refresh_bid_max();
				bidMaxChanged = true;
			}

			if (isOrderFilled) {
				if (bidMaxChanged) {
					parentEngine.queue_price_change(bookSecurityId, bidMax->price, askMin->price);
				}
				return FILLED;
			}
		}
	}
	return !FILLED;
};

bool Book::cross_market_order(Order& order) {
	if (order.side == BUY) {  // cross with asks
		bool askMinChanged = false;

		while (askMin->price <= MAX_PRICE) {
			order.price = askMin->price;
			bool isOrderFilled = match_at_limit(parentEngine, order, *askMin);
			bool isLimitFilled = askMin->volume == 0;

			if (isLimitFilled) {
				askTree.remove(askMin);
				refresh_ask_min();
				askMinChanged = true;
			}

			if (isOrderFilled) {
				if (askMinChanged) {
					parentEngine.queue_price_change(bookSecurityId, bidMax->price, askMin->price);
				}
				return FILLED;
			}
		}
	} else {  // cross with bids
		bool bidMaxChanged = false;

		while (bidMax->price >= MIN_PRICE) {
			order.price = bidMax->price;
			bool isOrderFilled = match_at_limit(parentEngine, order, *bidMax);
			bool isLimitFilled = bidMax->volume == 0;

			if (isLimitFilled) {
				bidTree.remove(bidMax);
				refresh_bid_max();
				bidMaxChanged = true;
			}

			if (isOrderFilled) {
				if (bidMaxChanged) {
					parentEngine.queue_price_change(bookSecurityId, bidMax->price, askMin->price);
				}
				return FILLED;
			}
		}
	}
	return !FILLED;
};

Order Book::cancel(const OrderId orderId) {
	auto entry = entries.get(orderId);
	if (entry) {
		auto originalOrder = entry->order;
		entry->cancel_order();

		if (entry->parentLimit->volume == 0) {
			if (originalOrder.side == BUY) {
				bidTree.remove(entry->parentLimit);
				refresh_bid_max();
			} else {
				askTree.remove(entry->parentLimit);
				refresh_ask_min();
			}
		}

		return originalOrder;
	}
	return Order();
};

// Private member functions

bool Book::match_at_limit(Engine& parentEngine, Order& order, Limit& limit) {
#ifdef _DEBUG
	if (order.side == BUY) {
		assert(order.price >= limit.head->order.price);
	} else {
		assert(order.price <= limit.head->order.price);
	}
#endif
	LimitEntry* currentEntry = limit.head;
	bool buySide = order.side == BUY;

	while (currentEntry) {
		Order& buyOrder = buySide ? order : currentEntry->order;
		Order& sellOrder = buySide ? currentEntry->order : order;
		Quantity execQuantity = std::min(order.quantity, currentEntry->order.quantity);

		if (execQuantity > 0) {	 // ignore cancelled orders
			parentEngine.execute(buyOrder, sellOrder, execQuantity);
			buyOrder.quantity -= execQuantity;
			sellOrder.quantity -= execQuantity;
			limit.decrease_volume(execQuantity);
		}

		bool orderFilled = order.quantity == 0;
		bool limitEntryFilled = currentEntry->order.quantity == 0;

		if (!orderFilled) {
			currentEntry = currentEntry->nextEntry;
		} else {
			update_limit(limitEntryFilled, limit, currentEntry);
			return FILLED;	// order filled
		}
	}

	limit.head = nullptr;
	limit.tail = nullptr;

	return !FILLED;	 // order unfilled / partially filled
}

void Book::update_limit_entries(Limit* limit, LimitEntry* newEntry, const Order& order) {
	if (limit->head) {
		limit->tail->nextEntry = newEntry;
		limit->tail = newEntry;
	} else {
		limit->head = newEntry;
		limit->tail = newEntry;

		if (order.side == BUY) {
			bidTree.insert(limit);
		} else {
			askTree.insert(limit);
		}
	}
#ifdef _DEBUG
	assert(limit->head);
	assert(limit->tail);
#endif
}

void Book::update_limit(const bool limitEntryFilled, Limit& limit, LimitEntry* limitEntry) {
	if (!limitEntryFilled) {
		limit.head = limitEntry;
	} else if (limitEntry->nextEntry) {
		limit.head = limitEntry->nextEntry;
	} else {
		limit.head = nullptr;
		limit.tail = nullptr;
	}
#ifdef _DEBUG
	if (!limit.head) {
		assert(!limit.tail);
	}
#endif
}

void Book::refresh_bid_max() { bidMax = bidTree.findMax(); }

void Book::refresh_ask_min() { askMin = askTree.findMin(); }
