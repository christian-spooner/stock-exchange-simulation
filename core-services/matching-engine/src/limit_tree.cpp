#include "limit_tree.hpp"

Node* LimitTree::makeEmpty(Node* t) {
	if (t == nullptr) return nullptr;
	{
		makeEmpty(t->left);
		makeEmpty(t->right);
		delete t;
	}
	return nullptr;
}

Node* LimitTree::insert(Limit* limptr, Node* t) {
	if (t == nullptr) {
		t = new Node;
		t->limit = limptr;
		t->left = t->right = nullptr;
	} else if (limptr->price < t->limit->price)
		t->left = insert(limptr, t->left);
	else if (limptr->price > t->limit->price)
		t->right = insert(limptr, t->right);
	return t;
}

Node* LimitTree::findMin(Node* t) {
	if (t == nullptr)
		return nullptr;
	else if (t->left == nullptr)
		return t;
	else
		return findMin(t->left);
}

Node* LimitTree::findMax(Node* t) {
	if (t == nullptr)
		return nullptr;
	else if (t->right == nullptr)
		return t;
	else
		return findMax(t->right);
}

Node* LimitTree::remove(Limit* limptr, Node* t) {
	Node* temp;
	if (t == nullptr)
		return nullptr;
	else if (limptr->price < t->limit->price)
		t->left = remove(limptr, t->left);
	else if (limptr->price > t->limit->price)
		t->right = remove(limptr, t->right);
	else if (t->left && t->right) {
		temp = findMin(t->right);
		t->limit = temp->limit;
		t->right = remove(t->limit, t->right);
	} else {
		temp = t;
		if (t->left == nullptr)
			t = t->right;
		else if (t->right == nullptr)
			t = t->left;
		delete temp;
	}

	return t;
}

Node* LimitTree::find(Node* t, Price price) {
	if (t == nullptr)
		return nullptr;
	else if (price < t->limit->price)
		return find(t->left, price);
	else if (price > t->limit->price)
		return find(t->right, price);
	else
		return t;
}

void LimitTree::in_order(Node* t) const {
	if (t == nullptr) return;
	in_order(t->left);
	std::cout << t->limit->price << " ";
	in_order(t->right);
}
