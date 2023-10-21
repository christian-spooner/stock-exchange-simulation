#pragma once

#include <cassert>

#include "data_types.hpp"

/**
 * A buffer to hold the limit entries for a given book.
 */
class LimitEntryBuffer {
   public:
	LimitEntryBuffer(const std::string& securityId) : securityId(securityId) {}

	~LimitEntryBuffer() {}

	/**
	 * Request a new limit entry.
	 * @param order The limit order to add to the entry.
	 * @param limit The parent limit of the order.
	 * @returns The new limit entry.
	 */
	LimitEntry* new_entry(const Order& order, Limit* limit) {
		assert(order.securityId == securityId);

		auto index = order.orderId % MAX_LIVE_ENTRIES;
		LimitEntry* obj = &buffer[index];
		if (obj) {
			obj->parentLimit = limit;
			obj->set_order(order);
		}
		return obj;
	}

	/**
	 * Get an existing limit entry.
	 * @param orderId The id of the requested order.
	 * @returns The reqeusted limit entry.
	 */
	LimitEntry* get(const OrderId orderId) {
		auto index = orderId % MAX_LIVE_ENTRIES;
		auto entry = &buffer[index];
		if (entry->parentLimit) {
			return entry;
		}
		return nullptr;
	}

   private:
	LimitEntry buffer[MAX_LIVE_ENTRIES + 1];
	std::string securityId;
};
