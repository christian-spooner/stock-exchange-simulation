#include <cassert>
#include <iostream>

#include "limit_tree.hpp"

void test_insert_and_find_min_and_max() {
	LimitTree tree;

	Limit limit1(10);
	tree.insert(&limit1);
	assert(tree.findMin()->price == 10);
	assert(tree.findMax()->price == 10);

	Limit limit2(5);
	tree.insert(&limit2);
	assert(tree.findMin()->price == 5);
	assert(tree.findMax()->price == 10);

	Limit limit3(15);
	tree.insert(&limit3);
	assert(tree.findMin()->price == 5);
	assert(tree.findMax()->price == 15);

	std::cout << "Test limit tree, 1: passed" << std::endl;
}

void test_remove() {
	LimitTree tree;

	Limit limit1(10);
	Limit limit2(5);
	Limit limit3(15);

	tree.insert(&limit1);
	tree.insert(&limit2);
	tree.insert(&limit3);

	tree.remove(&limit1);
	assert(tree.findMin()->price == 5);
	assert(tree.findMax()->price == 15);

	tree.remove(&limit3);
	assert(tree.findMin()->price == 5);
	assert(tree.findMax()->price == 5);

	tree.remove(&limit2);
	assert(tree.findMin() == nullptr);
	assert(tree.findMax() == nullptr);

	std::cout << "Test limit tree, 2: passed" << std::endl;
}

void test_search() {
	LimitTree tree;

	Limit limit1(10);
	Limit limit2(5);
	Limit limit3(15);

	tree.insert(&limit1);
	tree.insert(&limit2);
	tree.insert(&limit3);

	assert(tree.search(10)->price == 10);
	assert(tree.search(5)->price == 5);
	assert(tree.search(15)->price == 15);
	assert(tree.search(20) == nullptr);

	std::cout << "Test limit tree, 3: passed" << std::endl;
}

int test_limit_tree() {
	test_insert_and_find_min_and_max();
	test_remove();
	test_search();

	return 0;
}
