#include <cassert>
#include <iostream>

#include "test_limit_buffer.cpp"
#include "test_limit_entry_buffer.cpp"
#include "test_limit_tree.cpp"
#include "test_book.cpp"

int main() {
	test_limit_entry_buffer();
	test_limit_buffer();
	test_limit_tree();
    test_book();

	return 0;
}
