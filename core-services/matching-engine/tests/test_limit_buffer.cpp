#include <cassert>
#include <iostream>

#include "limit_buffer.hpp"

void test_get_limit() {
	LimitBuffer buff;

	Limit* l1 = buff.get(4800);
	assert(l1->price == 4800);

	Limit* l2 = buff.get(10);
	assert(l2->price == 10);

	Limit* l3 = buff.get(65535);
	assert(l3->price == 65535);

	std::cout << "Test limit buffer, 1: passed" << std::endl;
}

int test_limit_buffer() {
	test_get_limit();

	return 0;
}
