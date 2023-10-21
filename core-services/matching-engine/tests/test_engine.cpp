#include <cassert>
#include <iostream>

#include "engine.hpp"

Order o1 = {1, "MSFT", "FOO", 0, 100, 101};

Order o2 = {2, "MSFT", "BAR", 0, 100, 101};

Order o3 = {2, "MSFT", "BAR", 1, 100, 101};

Order o4 = {2, "MSFT", "BAR", 1, 50, 101};
Order o5 = {3, "MSFT", "BAR", 1, 50, 101};
Order o6 = {4, "MSFT", "BAR", 1, 50, 101};

Order o7 = {2, "MSFT", "BAR", 1, 50, 110};
Order o8 = {3, "MSFT", "BAR", 1, 50, 108};
Order o9 = {4, "MSFT", "FOO", 0, 50, 103};
Order o10 = {5, "MSFT", "BAR", 1, 50, 103};
Order o11 = {6, "MSFT", "FOO", 0, 50, 100};

void test_init() {
	execNum = 0;
	cancelNum = 0;
	priceNum = 0;
}

void test_add_limit_order() {
	test_init();
	Engine engine;
	engine.limit(o1);
	engine.limit(o2);
	std::cout << "Test 1: passed" << std::endl;
}

void test_cancel() {
	test_init();
	Engine engine;
	engine.limit(o1);
	engine.cancel(1, "MSFT");
	engine.cancel(2, "MSFT");
	assert(execNum == 0);
	assert(cancelNum == 1);
	std::cout << "Test 2: passed" << std::endl;
}

void test_match() {
	test_init();
	Engine engine;
	engine.limit(o1);
	engine.limit(o3);
	assert(execNum == 2);
	std::cout << "Test 3: passed" << std::endl;
}

void test_partial_fill() {
	test_init();
	Engine engine;
	engine.limit(o1);
	engine.limit(o4);
	engine.limit(o5);
	engine.limit(o6);
	assert(execNum == 4);
	std::cout << "Test 4: passed" << std::endl;
}

void test_price() {
	test_init();
	Engine engine;
	engine.limit(o1);
	engine.limit(o7);
	engine.limit(o8);
	engine.limit(o9);
	engine.limit(o10);
	engine.limit(o11);
	assert(execNum == 2);
	assert(priceNum == 4);
	std::cout << "Test 5: passed" << std::endl;
}

int main() {
	test_add_limit_order();
	test_cancel();
	test_match();
	test_partial_fill();
	test_price();

	return 0;
}
