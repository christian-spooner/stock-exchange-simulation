#include <cassert>
#include <iostream>

#include "constants.hpp"
#include "limit_entry_buffer.hpp"

void test_insert() {
	LimitEntryBuffer buffer("MSFT");

	Limit l1 = Limit(32000);

	Order o1 = Order(1, "MSFT", "FOO", 0, 100, 32000);
	LimitEntry* e1 = buffer.new_entry(o1, &l1);
	assert(e1->order.price == 32000);
	assert(e1->parentLimit->price == 32000);

	Order o2 = Order(2, "MSFT", "BAR", 0, 100, 32000);
	LimitEntry* e2 = buffer.new_entry(o2, &l1);
	assert(e2->order.price == 32000);
	assert(e2->parentLimit->price == 32000);

	Limit l2 = Limit(16000);

	Order o3 = Order(3, "MSFT", "FOO", 0, 100, 16000);
	LimitEntry* e3 = buffer.new_entry(o3, &l2);
	assert(e3->order.price == 16000);
	assert(e3->parentLimit->price == 16000);

	std::cout << "Test limit entry buffer, 1: passed" << std::endl;
}

void test_get() {
	LimitEntryBuffer buffer("MSFT");

	Order o1 = Order(1, "MSFT", "FOO", 0, 100, 32000);
	Limit l1 = Limit(32000);
	LimitEntry* e1 = buffer.new_entry(o1, &l1);
	Order o2 = Order(2, "MSFT", "BAR", 0, 100, 32000);
	buffer.new_entry(o2, &l1);

	LimitEntry* e1_get = buffer.get(1);
	assert(e1_get == e1);

	std::cout << "Test limit entry buffer, 2: passed" << std::endl;
}

void test_overwrite() {
	LimitEntryBuffer buffer("MSFT");

	Limit l = Limit(32000);

	for (int i = 0; i < MAX_LIVE_ENTRIES + 2; ++i) {
		Order o = Order(i, "MSFT", "FOO", 0, 100, 32000);
		buffer.new_entry(o, &l);
	}

	LimitEntry* e = buffer.get(0);
	assert(e->order.orderId == MAX_LIVE_ENTRIES);

	std::cout << "Test limit entry buffer, 3: passed" << std::endl;
}

int test_limit_entry_buffer() {
	test_insert();
	test_get();
	test_overwrite();

	return 0;
}
