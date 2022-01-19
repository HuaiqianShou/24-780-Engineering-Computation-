//#pragma warning(disable:4996)
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <ctype.h>
#include <string>

#include "fssimplewindow.h"
#include "GraphicFont.h"
#include "DrawingUtilNG.h"

#include "BST_hshou.h"


/*
Huaiqian Shou & Nestor Gomez
Carnegie Mellon University
Eng. Computation, 24-780-B
Prob Set 8
Due Tues. 11/16, 2021
Description: word Cloud using BST
*/


using namespace std;

void readFile(const string fileName, BST<string, TreeNode<string>>& tree);
void simplePrint(BST<string, TreeNode<string>>& tree);
void simplePrint2(BST<string, TreeNode<string>>& tree);
void printMenu();
int randBetween(int low, int high);
void setRandomCoords(int* xCoords, int* yCoords,   // call by reference on array
	int size, int minX, int maxX, int minY, int maxY);
void setRandomColors(int* colors, int size);

int main(void)
{
	BST<string, TreeNode<string>> myTree;
	readFile("Gettysburg.txt", myTree);
	//readFile("GettysburgShort.txt", myTree);
	//myTree.deleteItem("the");
	myTree.printInOrder(cout);
	//cout << "==========================================" << endl;
	//simplePrint(myTree);
	//cout << "==========================================" << endl;
	//simplePrint2(myTree);

	//cout << "The word 'nation' appears " << myTree.retrieveItemCount("nation") << " times." << endl;
	//cout << "The word 'superman' appears " << myTree.retrieveItemCount("superman") << " times." << endl;

	// the rest of the code is for implementing wordcloud

	using namespace DrawingUtilNG;
	int scrnWid = 1100, scrnHei = 800;
	// must "open window" before declaring fonts
	FsOpenWindow(16, 16, scrnWid, scrnHei, 1);

	int key = FSKEY_NULL;

	// select a font by uncommenting only one line
	//ImpactFont myFont;
	//GaramondFont myFont;
	//JokermanFont myFont;
	ComicSansFont myFont;
	//TimesNewRomanFont myFont;
	//OldEnglishFont myFont;
	//CourierNewFont myFont;
	//ArialFont myFont;

	int maxNodeCount = myTree.count();
	int currCount;
	string currString;
	TreeNode<string> currNode;
	float baseScale = 0.2;

	// generate random screen locations and colors
	int* randX = new int[maxNodeCount];
	int* randY = new int[maxNodeCount];
	int* randC = new int[maxNodeCount];

	setRandomCoords(randX, randY, maxNodeCount,
		scrnWid * 0.1, scrnWid * 0.7,
		scrnHei * 0.2, scrnHei * 0.8);
	setRandomColors(randC, maxNodeCount);

	bool rainbowColors = true;

	// tree of unwanted words
	BST<string, TreeNode<string>> unwantedTree;
	unwantedTree.insertItem("it");
	unwantedTree.insertItem("the");
	unwantedTree.insertItem("that");
	unwantedTree.insertItem("and");
	unwantedTree.insertItem("a");
	unwantedTree.insertItem("to");
	bool showAll = false;

	printMenu();

	while (FSKEY_ESC != key)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// draw black rectangle
		glColor3d(0.0, 0.0, 0.0);
		drawRectangle(0, 0, scrnWid, scrnHei, true);

		// draw words
		currNode = myTree.firstNode();
		for (int i = 0; i < maxNodeCount; i++) {
			currString = currNode.theItem;
			currCount = currNode.count;

			if (rainbowColors)
				myFont.setColorHSV(randC[i], 1, 1, .4);
			else
				myFont.setColorHSV(280, 1, 1, .4);

			if (currCount > 1 && (showAll || unwantedTree.retrieveItemCount(currString) == 0))
				myFont.drawText(currString, randX[i], randY[i],
					baseScale * (currCount - 1));

			currNode = myTree.nextNode();
		}

		FsPollDevice();
		switch (key = FsInkey()) {
		case FSKEY_UP: // zooming in
			baseScale *= 1.1;
			break;
		case FSKEY_DOWN: // zooming out
			baseScale /= 1.1;
			break;

		case FSKEY_U: // toggle unwanted words on or off
			showAll = !showAll;
			break;
		case FSKEY_C: // toggle rainbow colors on or off
			rainbowColors = !rainbowColors;
			break;

		case FSKEY_SPACE: // Re-order
		case FSKEY_R: // Re-order
			setRandomCoords(randX, randY, maxNodeCount,
				scrnWid * 0.1, scrnWid * 0.7,
				scrnHei * 0.2, scrnHei * 0.8);
			setRandomColors(randC, maxNodeCount);
			break;
		}

		FsSwapBuffers();
		FsSleep(30);

	}

	// just to be nice and polite
	delete[]randX;
	delete[]randY;
	delete[]randC;
}

// from Lecture ??
std::vector<std::string> split(const std::string str, std::string delim) {
	char* cstr = const_cast<char*>(str.c_str());
	char* current;
	std::vector<std::string> arr;
	current = strtok(cstr, delim.c_str());
	while (current != NULL) {
		arr.push_back(current);
		current = strtok(NULL, delim.c_str());
	}
	return arr;
}

string tolower(const string& in) {
	string out = in;
	for (auto i = 0; i < in.length(); i++)
		out[i] = tolower(in[i]);    // returns the lower case of char
	return out;
}

void readFile(string fileName, BST<string, TreeNode<string>>& tree)
{
	string tempStr;
	string delim = " .,;:!?\"\'[]()|\t\n";

	ifstream inFile(fileName);
	if (inFile.is_open()) { // may not have found file

		while (!inFile.eof()) {
			// read whole line
			getline(inFile, tempStr);

			// split line into words
			vector<string> tokens = split(tempStr, delim);

			// insert each of the words into BST (faster to do it "backwards" when popping)
			while (!tokens.empty()) {
				tree.insertItem(tolower(tokens.back()));
				tokens.pop_back();
			}
		}
		inFile.close();
	}
}

// for debugging
void simplePrint(BST<string, TreeNode<string>>& tree)
{
	int maxNodeCount = tree.count();
	TreeNode<string> currNode = tree.firstNode();
	cout << currNode.theItem << "{" << currNode.count << ", " << 0 << "}" << endl;
	for (int i = 0; i < maxNodeCount; i++) {
		currNode = tree.nextNode();
		cout << currNode.theItem << "{" << currNode.count << ", " << i << "}" << endl;
	}
}

// for debugging
void simplePrint2(BST<string, TreeNode<string>>& tree)
{
	int maxNodeIndex = tree.size();
	TreeNode<string> currNode;
	for (int i = 0; i < maxNodeIndex; i++) {
		currNode = tree.theTree[i];
		if (currNode.count != 0)
			cout << currNode.theItem << "[" << i << "]" << endl;
	}
}

void printMenu()
{
	cout << endl << endl << "Instructions: " << endl;
	cout << "    Arrow UP to zoom in." << endl;
	cout << "    Arrow DOWN to zoom out." << endl;
	cout << "    'U' to toggle display of \"common\" words." << endl;
	cout << "    'C' to toggle rainbow colors mode." << endl;
	cout << "    'R' or SPACE to regenerate graphics." << endl;
	cout << endl;
}

int randBetween(int low, int high) {
	return low + ((double)rand() / RAND_MAX * (high - low));
}

void setRandomCoords(int* xCoords, int* yCoords,   // call by reference
	int size, int minX, int maxX, int minY, int maxY) {
	for (int i = 0; i < size; i++) {
		xCoords[i] = randBetween(minX, maxX);
		yCoords[i] = randBetween(minY, maxY);
	}
}

void setRandomColors(int* colors, int size) {
	for (int i = 0; i < size; i++) {
		colors[i] = randBetween(0, 35) * 10; // 0 to 350 by tens
	}
}
