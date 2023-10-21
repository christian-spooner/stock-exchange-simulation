#include <cassert>
#include <iostream>

#include "book.hpp"

void test_queue() {
	Order o1 = {1, "MSFT", "FOO", 0, 100, 120};
	Order o2 = {2, "MSFT", "BAR", 0, 100, 121};
	Order o3 = {3, "MSFT", "BAR", 1, 100, 122};

	Engine engine;
	std::string securityId = "MSFT";
	auto book = new Book(engine, securityId);

	book->queue(o1);
	book->queue(o2);
	book->queue(o3);

	assert(book->get_bid_max()->price == 121);
	assert(book->get_ask_min()->price == 122);
	delete book;

	std::cout << "Test book, 1: passed" << std::endl;
}

void test_cancel() {
	Order o1 = {1, "MSFT", "FOO", 0, 100, 120};
	Order o2 = {2, "MSFT", "BAR", 0, 100, 122};

	Engine engine;
	std::string securityId = "MSFT";
	auto book = new Book(engine, securityId);

	book->queue(o1);
	book->queue(o2);
	book->cancel(2);

	assert(book->get_bid_max()->price == 120);
	delete book;

	std::cout << "Test book, 2: passed" << std::endl;
}

void test_cross_limit() {
	Order o1 = {1, "MSFT", "FOO", 0, 100, 120};
	Order o2 = {2, "MSFT", "BAR", 0, 100, 122};
	Order o3 = {3, "MSFT", "FOO", 1, 100, 120};

	Order o4 = {4, "MSFT", "BAR", 1, 100, 130};
	Order o5 = {5, "MSFT", "FOO", 1, 100, 132};
	Order o6 = {6, "MSFT", "FOO", 0, 100, 136};

	Engine engine;
	std::string securityId = "MSFT";
	auto book = new Book(engine, securityId);

	book->queue(o1);
	assert(book->get_bid_max()->price == 120);
	book->queue(o2);
	book->cross_limit_order(o3);
	assert(book->get_bid_max()->price == 120);

	book->queue(o4);
	assert(book->get_ask_min()->price == 130);
	book->queue(o5);
	book->cross_limit_order(o6);
	assert(book->get_ask_min()->price == 132);

	delete book;

	std::cout << "Test book, 3: passed" << std::endl;
}

void test_cross_market() {
	Order o1 = {1, "MSFT", "FOO", 0, 100, 120};
	Order o2 = {2, "MSFT", "BAR", 0, 100, 122};
	Order o3 = {3, "MSFT", "FOO", 1, 100, 0};

	Order o4 = {4, "MSFT", "FOO", 1, 100, 130};
	Order o5 = {5, "MSFT", "BAR", 1, 100, 132};
	Order o6 = {6, "MSFT", "FOO", 0, 100, 0};

	Engine engine;
	std::string securityId = "MSFT";
	auto book = new Book(engine, securityId);

	book->queue(o1);
	book->queue(o2);
	assert(book->get_bid_max()->price == 122);
	book->cross_market_order(o3);
	assert(book->get_bid_max()->price == 120);

	book->queue(o4);
	assert(book->get_ask_min()->price == 130);
	book->queue(o5);
	book->cross_market_order(o6);
	assert(book->get_ask_min()->price == 132);

	delete book;

	std::cout << "Test book, 4: passed" << std::endl;
}

int test_book() {
	test_queue();
	test_cancel();
	test_cross_limit();
	test_cross_market();

	return 0;
}
