#include <vector>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include "Shape.h"
#include "fssimplewindow.h"
#include "Node.h"
#include "DrawingUtilNG.h"
#include "ViewManager.h"
#include "ysglfontdata.h"

using namespace std;

void Shape::readFile(ifstream& inFile)
{
	string currLine;
	stringstream currStream;
	double xCoord, yCoord, dist;

	theGuides.clear();

	while (!inFile.eof()) {
		getline(inFile, currLine);
		if (currLine != "") {
			currStream.str(currLine);
			currStream >> xCoord >> yCoord;  // don't care about  >> dist;
			currStream.clear();

			Node newNode(xCoord, yCoord, -1);

			theGuides.push_back(newNode); // add copy of the node to model
		}
	}

	// reset smooth curve
	recalcSpline();
}

double Shape::getDistanceFromXY(double x, double y)
{
	return 0.0;
}

void Shape::getXYfromDistance(double distance, double& xCoord, double& yCoord)
{ // needed?
	int index = getIndexFromDistanceSpline(distance);
	if (index != -1) {
		double currX = theSpline[index].getX();
		double currY = theSpline[index].getY();
		double currD = theSpline[index].getDistance();
		double nextX = theSpline[index + 1].getX();
		double nextY = theSpline[index + 1].getY();
		double nextD = theSpline[index + 1].getDistance();
		double ratio = (distance - currD) / (nextD - currD);

		xCoord = currX + ratio * (nextX - currX);
		yCoord = currY + ratio * (nextY - currY);
	}
	else {
		xCoord = -INFINITY;
		yCoord = -INFINITY;
	}
}

double Shape::getSlopeAngle(double distance)
{
	// find segment on spline that encompasses distance
	int index = 0;
	if (distance < 0)
		distance = 0;

	if (distance > theSpline.back().getDistance())
		index = theSpline.size() - 1;
	else {
		while (index < theSpline.size() && theSpline[index].getDistance() <= distance)
			index++;
	}

	// calculate angle of segment and convert to degrees
	double slope = atan2(theSpline[index].getY() - theSpline[index - 1].getY(),
		theSpline[index].getX() - theSpline[index - 1].getX()) * 45. / atan(1.);
	if (slope < 0)
		return slope + 360.;
	else
		return slope;
}

int Shape::getIndexFromDistance(double distance)
{
	if (theGuides.back().getDistance() < distance && distance < getSplineLength())
		return theGuides.size();
	if (0 <= distance && distance <= theGuides.back().getDistance())
		for (int i = 1; i < theGuides.size(); i++) {
			if (theGuides[i].getDistance() > distance)
				return i - 0;
		}

	return -1;
}

int Shape::getIndexFromDistanceSpline(double distance)
{
	if (0 <= distance && distance <= theSpline.back().getDistance())
		for (int i = 1; i < theSpline.size(); i++) {
			if (theSpline[i].getDistance() > distance)
				return i - 1;
		}

	return -1;
}

void Shape::insertNode(Node& newNode, int index)
{
	if (index < 0)
		index = 0;

	if (index >= theGuides.size())
		theGuides.push_back(newNode);
	else
		theGuides.insert(theGuides.begin() + index, newNode);  // no longer need the + 1

	recalcSpline();
}

Node* Shape::findNode(double x, double y, double distance, bool searchSpline)
{
	double minX = x - distance, maxX = x + distance;
	double minY = y - distance, maxY = y + distance;
	double currX, currY;

	vector<Node>* searchVector;
	if (searchSpline)
		searchVector = &theSpline;
	else
		searchVector = &theGuides;

	for (auto& currNode : *searchVector) {
		currX = currNode.getX();
		currY = currNode.getY();

		if (minX < currX && currX < maxX && minY < currY && currY < maxY)
			return &(currNode);
	}

	return nullptr;
}

void Shape::draw(int shapeColor, int lineWidth, int nodeColor, 
	bool showNodes, bool showAxes)
{
	if (theGuides.size() == 0) {
		glColor3f(1, 0, 0);
		glRasterPos2d(10, 40);
		YsGlDrawFontBitmap16x20("Model is empty.");
	}
	else {

		glLineWidth(lineWidth);
		double red, green, blue;
		DrawingUtilNG::hsv2rgb(shapeColor, 1, 1, red, green, blue);
		glColor3d(red, green, blue);

		if (shapeFilled) {
			// using stencil buffer
			// https://stackoverflow.com/questions/25422846/how-to-force-opengl-to-draw-a-non-convex-filled-polygon
			glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_STENCIL_TEST);

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glStencilFunc(GL_ALWAYS, 0, 1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
			glStencilMask(1);

			// triangle fan starting at 0,0 and using every point in shape, 
			// including closing it by using the first point
			glBegin(GL_TRIANGLE_FAN);
			//theManager.screenVertex(0., 0.);
			glVertex2f(0., 0.);

			for (auto& currNode : theGuides)
				//theManager.screenVertex(currNode.getX(), currNode.getY());
				glVertex2f(currNode.getX(), currNode.getY());

			// do first point to "close the loop"
			//theManager.screenVertex(theGuides.front().getX(), theGuides.front().getY());
			glVertex2f(theGuides.front().getX(), theGuides.front().getY());
			glEnd();

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glStencilFunc(GL_EQUAL, 1, 1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			// draw something over whole area
			glBegin(GL_QUADS);
			//theManager.screenVertex(minX, minY);
			//theManager.screenVertex(maxX, minY);
			//theManager.screenVertex(maxX, maxY);
			//theManager.screenVertex(minX, maxY);
			glVertex2f(minX, minY);
			glVertex2f(maxX, minY);
			glVertex2f(maxX, maxY);
			glVertex2f(minX, maxY);
			glEnd();

			glDisable(GL_STENCIL_TEST);

			//// "wrong" way
			//glBegin(GL_POLYGON);
			//for (auto& currNode : theGuides)
			//	theManager.screenVertex(currNode.getX(), currNode.getY());

			//glEnd();

			glColor3ub(0, 0, 0);  // set black for boundary
		}
		glBegin(GL_LINE_LOOP);
		for (auto& currNode : theGuides)
			//theManager.screenVertex(currNode.getX(), currNode.getY());
			glVertex2f(currNode.getX(), currNode.getY());

		glEnd();
		glLineWidth(1);

		// show guideNodes
		if (showNodes) {
			DrawingUtilNG::hsv2rgb(nodeColor, 1, 1, red, green, blue);
			glColor3d(red, green, blue);
			for (auto& currNode : theGuides) {
				//currNode.draw(theManager, lineWidth + 4, false);
				currNode.draw(lineWidth + 4, false);
			}
		}
		// show neutral axes
		if (showAxes) {
			DrawingUtilNG::hsv2rgb(240., 0.0, 0.5, red, green, blue);
			glColor3d(red, green, blue);
			glLineWidth(2);
			glBegin(GL_LINES);
			glVertex2f(minX, centroidY);
			glVertex2f(maxX, centroidY);
			glVertex2f(centroidX, minY);
			glVertex2f(centroidX, maxY);
			glEnd();
			glLineWidth(1);
		}
	}
}

void Shape::writeFile(std::ostream& output)
{
	for (auto& currNode : theGuides)
		output << currNode << endl;
}

void Shape::clearAll()
{
	theGuides.clear();
	theSpline.clear();
}

double Shape::getSplineLength()
{
	if (theSpline.size() > 0)
		return theSpline.back().getDistance();
	else
		return -1;
}

bool Shape::deleteNode(Node* badNode)
{
	for (int i = 0; i < theGuides.size(); i++) {
		if (badNode == &theGuides[i]) {
			theGuides.erase(theGuides.begin() + i);
			//recalcSpline();
			return true;
		}
	}
	return false;
}

void Shape::recalcSpline()
{
	theSpline.clear();
	minX = minY = 0;
	maxX = maxY = 10;
	double currDist;

	// now get the distances
	if (theGuides.size() > 2) {
		currDist = 0;
		theGuides[0].setDistance(0);
		minX = min(theGuides[0].getX(), theGuides[1].getX());
		minY = min(theGuides[0].getY(), theGuides[1].getY());
		maxX = max(theGuides[0].getX(), theGuides[1].getX());
		maxY = max(theGuides[0].getY(), theGuides[1].getY());

		for (int i = 1; i < theGuides.size(); i++) {
			currDist += DrawingUtilNG::getDistance(theGuides[i].getX(), theGuides[i].getY(),
				theGuides[i - 1].getX(), theGuides[i - 1].getY());
			theGuides[i].setDistance(currDist);

			// check boundary
			minX = min(minX, theGuides[i].getX());
			minY = min(minY, theGuides[i].getY());
			maxX = max(maxX, theGuides[i].getX());
			maxY = max(maxY, theGuides[i].getY());
		}

		// something nice to add in to take care of tolerances and adjust for overall size
		tolerance = min(maxX - minX, maxY - minY) * 1e-8;

		// instead of generating spline points, just add in-between points for each segment
		Node prevNode = theGuides[0];
		Node currNode;
		int splitCount = 6;
		double currDist = 0., segLength, deltaX, deltaY;
		for (int i = 1; i < theGuides.size(); i++) {
			currNode = theGuides[i];
			deltaX = currNode.getX() - prevNode.getX();
			deltaY = currNode.getY() - prevNode.getY();
			segLength = DrawingUtilNG::getDistance(currNode.getX(), currNode.getY(), prevNode.getX(), prevNode.getY());
			for (int j = 0; j < splitCount; j++) {
				theSpline.push_back(Node(prevNode.getX() + deltaX * j / splitCount, 
					prevNode.getY() + deltaY * j / splitCount, currDist + segLength * j / splitCount) );
			}
			prevNode = currNode;
			currDist += segLength;
		}
		calcProperties();

		/*		if (theGuides.size() >= 4) {

					// first and last guide points are for sloping the first part of curve
					//for (int i = 1; i < theGuides.size() - 2; i++) {

					// need all points on spline
					for (int i = 0; i < theGuides.size(); i++) {

						// generate spline points
						segmentPoints(i);

					}

					// now get the distances
					double currDist = 0;
					theSpline[0].setDistance(0);
					for (int i = 1; i < theSpline.size(); i++) {
						currDist += DrawingUtilNG::getDistance(theSpline[i].getX(), theSpline[i].getY(),
							theSpline[i - 1].getX(), theSpline[i - 1].getY());
						theSpline[i].setDistance(currDist);
					}
				}
		*/
	}
}

bool Shape::isContained(double testX, double testY, bool isInclusive)
{
	// check bounding box
	if (testX < minX || testX > maxX || testY < minY || testY > maxY)
		return false;
	else {
		// initialize count of ray crossing
		int crossCount = 0;
		// go through each segment, 
		// starting with segment that goes from "last" point to "first" point
		double currX, currY;
		double prevX = theGuides.back().getX();
		double prevY = theGuides.back().getY();
		for (int i = 0; i < theGuides.size(); i++) {
			currX = theGuides[i].getX();
			currY = theGuides[i].getY();

			// now I have a line segment from (prevX, prevY) to (currX, currY)

			// check if segment should be ignored 
			if ((prevY < testY && currY < testY)  // segment is too low
				|| (prevY > testY && currY > testY)  // segment is too high
				|| (prevX < testX && currX < testX)) {  // segment is to the left of test point
				// do nothing
			}
			else {
				// check if test point is at current vertex
				if (abs(testX - currX) < tolerance && abs(testY - currY) < tolerance)
					return isInclusive;     // POSSIBLE EXIT FROM FUNCTION

				// check if ray goes through either vertex
				else if ((abs(testY - prevY) < tolerance && currY > testY)
					|| (abs(testY - currY) < tolerance && prevY > testY))
					crossCount++;
				else {
					// get intersection of segment and ray
					double interX, interY;
					DrawingUtilNG::getIntersection(interX, interY,
						prevX, prevY, currX, currY, testX, testY, testX + 100, testY);
					if (abs(interX - testX) < tolerance) // test point is on segment itself
						return isInclusive;     // POSSIBLE EXIT FROM FUNCTION
					else if (interX > testX)
						crossCount++;
				}

			}

			// advance to next segment
			prevX = currX;
			prevY = currY;
		}
		return (crossCount % 2 == 1);  // return true if crossCount is odd
	}
}
// I used the equations from
// http://richardson.eng.ua.edu/Former_Courses/CE_331_fa09/Projects/A_and_I_of_Polygon.pdf
// and parallel axis theorem to calcualte the area, centroidX, centroidY 
// and the moments of inertia about the orthogonal neutral axes.
void Shape::calcProperties()
{
	int numOfnodes = this->theGuides.size();
	double sumForA = 0;
	double sumForCx = 0;
	double sumForCy = 0;
	double Ix = 0;
	double sumForIx = 0;
	double Iy = 0;
	double sumForIy = 0;

	for (int i = 0; i < numOfnodes; i++) {
		double cor_x = 0.0;
		double cor_y = 0.0;
		double cor_x_next = 0.0;
		double cor_y_next = 0.0;
		// make sure loop through all the nodes and stop when loop back to the first node (instead of the last)
		if (i == numOfnodes - 1) {
			cor_x = this->theGuides.at(i).getX();
			cor_y = this->theGuides.at(i).getY();
			cor_x_next = this->theGuides.at(0).getX();
			cor_y_next = this->theGuides.at(0).getY();
		}
		else {
			cor_x = this->theGuides.at(i).getX();
			cor_y = this->theGuides.at(i).getY();
			cor_x_next = this->theGuides.at(i + 1).getX();
			cor_y_next = this->theGuides.at(i + 1).getY();
		}
		
		double A_tri = cor_x * cor_y_next - cor_x_next * cor_y;
		sumForA += cor_x * cor_y_next - cor_x_next * cor_y;
		sumForCx += (cor_x + cor_x_next) * A_tri;
		sumForCy += (cor_y + cor_y_next) * A_tri;
		sumForIx += (cor_y * cor_y + cor_y * cor_y_next + cor_y_next * cor_y_next) * A_tri;
		sumForIy += (cor_x * cor_x + cor_x * cor_x_next + cor_x_next * cor_x_next) * A_tri;
	}

	this->area = sumForA / 2.0;
	this->centroidX = (1 / (6 * this->area)) * sumForCx;
	this->centroidY = (1 / (6 * this->area)) * sumForCy;
	Ix = (1 / 12.0) * sumForIx;
	Iy = (1 / 12.0) * sumForIy;
	this->momX0 = Ix-this->area*this->centroidY*this->centroidY;
	this->momY0 = Iy - this->area * this->centroidX * this->centroidX;;

}
// This function print all the porperties of the current shape
void Shape::printProperties()
{
	cout << "Area: " << this->area << endl;
	cout << "CentroidX: " << this->centroidX << endl;
	cout << "CentroidY: " << this->centroidY << endl;
	cout << "MomX0: " << this->momX0 << endl;
	cout << "MomY0: " << this->momY0 << endl;
}

void Shape::segmentPoints(int segIndex)
{
	double c = splineTension; // spline tension (0.0 means no spline at all)
	//double c = 0.;   // spline tension (0.0 means no spline at all)

	// adjustments for closed loop
	int nextIndex = (segIndex + 1) % theGuides.size();
	int nextNextIndex = (segIndex + 2) % theGuides.size();

	int prevIndex = segIndex - 1;
	if (prevIndex < 0)
		prevIndex = theGuides.size() - 1;

	Node prev = theGuides[prevIndex];
	Node curr = theGuides[segIndex];
	Node next = theGuides[nextIndex];
	Node nextNext = theGuides[nextNextIndex];


	double xa = -c * prev.getX() + (2. - c) * curr.getX()
		+ (c - 2.) * next.getX() + c * nextNext.getX();
	double xb = 2. * c * prev.getX() + (c - 3.) * curr.getX()
		+ (3. - 2. * c) * next.getX() - c * nextNext.getX();
	double xc = -c * prev.getX() + c * next.getX();
	double xd = curr.getX();

	double ya = -c * prev.getY() + (2. - c) * curr.getY()
		+ (c - 2.) * next.getY() + c * nextNext.getY();
	double yb = 2. * c * prev.getY() + (c - 3.) * curr.getY()
		+ (3. - 2. * c) * next.getY() - c * nextNext.getY();
	double yc = -c * prev.getY() + c * next.getY();
	double yd = curr.getY();

	// add the spline points

	theSpline.push_back(curr);
	double t, x, y;
	for (int k = 1; k <= splineSubdivisions; k++) {
		t = double(k) / splineSubdivisions;  // parameter
		x = xa * t * t * t + xb * t * t + xc * t + xd;
		y = ya * t * t * t + yb * t * t + yc * t + yd;
		//Node newNode(x, y);
		theSpline.push_back(Node(x, y));
	}

}

