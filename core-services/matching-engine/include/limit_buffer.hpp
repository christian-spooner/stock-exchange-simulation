#pragma once

#include "data_types.hpp"

/**
 * A buffer to hold the price limits of an order book.
 */
class LimitBuffer {
   public:
	LimitBuffer() {
		for (int i = 0; i <= MAX_PRICE + 1; i++) {
			buffer[i].price = i;
			buffer[i].head = nullptr;
		}
	}

	Limit* get(const Price price) { return &buffer[price]; }

   private:
	Limit buffer[MAX_PRICE + 2];
};
