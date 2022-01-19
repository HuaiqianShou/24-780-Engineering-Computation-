/*
Nestor Gomez
Carnegie Mellon University
Eng. Computation, 24-780-B
Prob Set 6
Due Mon. Oct. 26, 2020
*/

#pragma once

#include <string>
//#include <iostream>

//using namespace std;  // removed so that other files are not forced into it

class ViewManager;  // placeholder for class ViewManager

class Node {
protected:
	double x, y;   // coordinates of the node
	double distance;  // a measure from the starting point

public:
	Node() {}
	Node(double xCoord, double yCoord, double dist = -1.);
	// default constructor for the class, setting all parameters.

	// get and set for all member variables
	double getX() { return x; }
	double getY() { return y; }
	double getDistance() { return distance; }

	void setX(double xCoord) { x = xCoord; }
	void setY(double yCoord) { y = yCoord; }
	void setXY(double xCoord, double yCoord) { x = xCoord; y = yCoord; }
	void setDistance(double dist) { distance = dist; }

	// draws a symbol (programmer's choice) of the node, filled as given
	// uses size (default = 2) to determine model size of node symbol
	// includes label a bit off-center if showLabel is true	
	// 
	// uses parameter theManager to access the getScreenCoords() function
	// to place the label and perhaps assure node symbol does not scale
	void draw(int size = 2,
		bool filled = true, bool showLabel = false);

	//writes all the node data to an output stream so that it looks just like an input file.
	friend std::ostream& operator<<(std::ostream& os, const Node& aNode);

};