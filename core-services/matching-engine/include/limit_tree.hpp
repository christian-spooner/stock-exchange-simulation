#pragma once

#include <iostream>
#include <optional>

#include "data_types.hpp"

struct Node {
	Limit* limit;
	Node* left;
	Node* right;
};

class LimitTree {
   public:
	LimitTree() { root = nullptr; }

	~LimitTree() { root = makeEmpty(root); }

	void insert(Limit* limptr) { root = insert(limptr, root); }

	void remove(Limit* limptr) { root = remove(limptr, root); }

	void display() const {
		in_order(root);
		std::cout << std::endl;
	}

	Limit* search(const Price price) {
		auto limitNode = find(root, price);
		if (limitNode) {
			return limitNode->limit;
		}

		return nullptr;
	}

	Limit* findMax() {
		auto maxNode = findMax(root);
		if (maxNode) {
			return maxNode->limit;
		}

		return nullptr;
	}

	Limit* findMin() {
		auto minNode = findMin(root);
		if (minNode) {
			return minNode->limit;
		}

		return nullptr;
	}

   private:
	Node* root;
	Node* makeEmpty(Node* t);
	Node* insert(Limit* limptr, Node* t);
	Node* findMin(Node* t);
	Node* findMax(Node* t);
	Node* remove(Limit* limptr, Node* t);
	Node* find(Node* t, Price price);
	void in_order(Node* t) const;
};
