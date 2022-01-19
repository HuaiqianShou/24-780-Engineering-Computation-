#pragma once

#include "Shape.h"


class Ishape : public Shape {

protected:
	float locX;
	float locY;
	float height;
	float width;
	float flangeT;
	float webT;
	float theta;

public:

	// clears old data and reads points from a file stream
	virtual void readFile(std::ifstream& inFile);

	// given model coordinates x,y , function returns the first guide point (or spline point)
	// that is within given distance
	// returns nullptr if no node meets the criteria
	virtual Node* findNode(double x, double y, double distance, bool searchSpline = false) {
		return Shape::findNode(x, y, distance, false);
	}


	// writes all data in the model to the given stream 
	virtual void writeFile(std::ostream& output);

	// creates points for shape
	virtual void recalcSpline();


};