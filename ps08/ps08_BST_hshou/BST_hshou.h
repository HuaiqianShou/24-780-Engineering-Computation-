#pragma warning(disable:4996)  // this helps with _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <fstream>
#include <vector>


/*
Huaiqian Shou & Nestor Gomez
Carnegie Mellon University
Eng. Computation, 24-780-B
Prob Set 8
Due Tues. 11/16, 2021
Description: BST class
*/

using namespace std;

// note, class "T" must be comparable using <, ==, and >
template <class T>
class TreeNode {
public:
	T theItem;
	int count;
	TreeNode() { count = 0; }
	TreeNode(const T& newItem) {
		theItem = newItem; count = 1;
	}
};

template <class T, class TN>
class BST
{
protected:
	int currLoc;
	int nodeCount = 0;
	void deleteItem(int deleteLoc);

	void moveUp(int location, int delta = -1);

public:  // this should be protected, but left it public for now
	std::vector<TN> theTree;

public:
	int size() { return theTree.size(); }
	int count() { return nodeCount; }
	const TN& firstNode();
	const TN& nextNode();
	void deleteItem(const T& anItem);

	// Create the code for these four functions

	// Used to insert item into BST. If newItem is invalid, do nothing
	// If item already in BST, update the item's count
	// If item is not already in BST, place it at the correct location and increment 
	// the nodeCount
public:	void insertItem(const T& newItem, int location = 0);

	  // Find the item and return the item count (makes use of findItem() function)
	  // Returns zero if the item is not in the BST
public:	int retrieveItemCount(const T& anItem);

	  // Tranverses the BST and prints out (to file or console) each of the items
	  // in the BST in correct order, along with the item's count (can use recursion)
public:	void printInOrder(ostream& out, int location = 0);

	  // Used internally by deleteItem() and others to get the vector index of the item
	  // Optionally starts the search at a node that is not the root, allowing for recursive
	  // searching (although search can be done without recursion, ignoring second parameter)
protected: 	int findItem(const T& anItem, int startLoc = 0);

};

// Including function definitions in .h because linker has trouble
// using template classes if compiler cannot access at compile time

template <class T, class TN>
void BST<T, TN>::moveUp(int location, int delta)
{
	// moves subtree up one level
	// caution: may over-ride part of tree, so only use for node with no siblings

	if (location > 0) {  // root cannot be moved

		// don't move node if node is empty or not in tree
		if (location < theTree.size() && theTree[location].count != 0) {
			// if original move location (not from a recursize call)
			// calculate distance between original node and parent node
			if (delta == -1)
				delta = location / 2 + 1;

			// move node up one level (by copying the node)
			theTree[location - delta] = theTree[location];

			// need to clear node as I move it up (in case it was a leaf)
			theTree[location] = TN();

			// move left subtree up one level 
			// (note that delta is NOT calculated as above)
			moveUp(2 * location + 1, delta * 2);

			// move right subtree up one level
			moveUp(2 * location + 2, delta * 2);
		}
	}
}


template <class T, class TN>
void BST<T, TN>::deleteItem(const T& anItem)
{
	// find item and remove from tree
	// overall approach: 
	//    find the selected item, clear it (set count to zero), and remember its index 
	//    if no children, you're done
	//    if one child, move child's subtree up to selected
	//    if two children, navigate to the right-most node of left child
	//    swap this node with the one located above and delete it
	//        note that the swapped node will either have zero or one child

	int location = findItem(anItem);
	if (location >= 0) {
		theTree[location] = TN();  // blank out the node
		nodeCount--;
		deleteItem(location);
	}
}

// This function inserts item to the tree
// Follow Professor's way and code by myself
template<class T, class TN>
inline void BST<T, TN>::insertItem(const T& newItem, int location)
{
	// 1) Create a new node(with key and data if applicable)
	if (newItem != "") {

		TN newNode(newItem);
		if (theTree.empty())
			theTree.push_back(newNode);
		else {
			// 2) Start with the root as the current node
			if (location >= theTree.size()) {
				theTree.resize(location + 1);
			}
			// 3) Compare the new node with the current node (using keys):
			if (theTree.at(location).count == 0) {
				theTree.at(location) = newNode;
				nodeCount++;
			}
			// 4) If new node is equal to current node, decide action and DONE
			else if (newItem == theTree.at(location).theItem)
				theTree.at(location).count++;
			// 5) If new node is less than current node:
			else if (newItem < theTree.at(location).theItem)
				// insert into left child
				insertItem(newItem, 2 * location + 1);
			else
				// insert into right child
				insertItem(newItem, 2 * location + 2);
		}
	}
}

template<class T, class TN>
inline int BST<T, TN>::retrieveItemCount(const T& anItem)
{
	int itemIndex = findItem(anItem);


	if (itemIndex > 0)
		return theTree.at(itemIndex).count;
	else
		return 0;
}

template<class T, class TN>
inline void BST<T, TN>::printInOrder(ostream& out, int location)
{
	if (!theTree.empty() && location < theTree.size() && theTree.at(location).count > 0) {
		//print the left child
		printInOrder(out, 2 * location + 1);
		//print the node
		out << theTree.at(location).theItem << "{" << theTree.at(location).count << "}" << endl;
		//print the right child
		printInOrder(out, 2 * location + 2);
	}


}

template<class T, class TN>
inline int BST<T, TN>::findItem(const T& anItem, int startLoc)
{
	//iterative approach

	int current = 0;
	bool Item_found = false;

	while (!Item_found && current < theTree.size() && theTree.at(current).count > 0) {
		if (anItem == theTree.at(current).theItem)
			Item_found = true;
		else if (anItem < theTree.at(current).theItem)
			current = 2 * current + 1;
		else
			current = 2 * current + 2;

	}
	if (Item_found)
		return current;
	else
		return -1;
}

template <class T, class TN>
void BST<T, TN>::deleteItem(int deleteLoc)
{
	// if there are two children 
	if ((deleteLoc * 2 + 2) < theTree.size()
		&& theTree[deleteLoc * 2 + 1].count > 0
		&& theTree[deleteLoc * 2 + 2].count > 0) {

		// need to find node to swap into deleteLoc
		// start with left child and find the right-most node
		int currLoc = deleteLoc * 2 + 1;  // the left child
		while ((currLoc * 2 + 2) < theTree.size()
			&& theTree[currLoc * 2 + 2].count > 0) {
			currLoc = currLoc * 2 + 2;
		}
		// swap right-most node with delete location
		theTree[deleteLoc] = theTree[currLoc];

		// delete the right-most node
		deleteItem(currLoc);
	}
	// if there is a child on left only
	else if ((deleteLoc * 2 + 2) < theTree.size()
		&& theTree[deleteLoc * 2 + 1].count > 0
		&& theTree[deleteLoc * 2 + 2].count == 0)
		moveUp(deleteLoc * 2 + 1);
	// if there is a child on right only
	else if ((deleteLoc * 2 + 2) < theTree.size()
		&& theTree[deleteLoc * 2 + 1].count == 0
		&& theTree[deleteLoc * 2 + 2].count > 0)
		moveUp(deleteLoc * 2 + 2);
}

template <class T, class TN>
const TN& BST<T, TN>::firstNode()
{
	currLoc = 0;
	return theTree.at(0);
}

template <class T, class TN>
const TN& BST<T, TN>::nextNode()
{
	currLoc++;
	while (currLoc < theTree.size() && theTree.at(currLoc).count == 0)
		currLoc++;
	return theTree.at(currLoc);
}

#pragma once
