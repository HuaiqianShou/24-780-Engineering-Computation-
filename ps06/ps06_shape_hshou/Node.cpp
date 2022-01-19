#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "Node.h"
#include "ViewManager.h"
#include "StringPlus.h"

using namespace std;

Node::Node(double xCoord, double yCoord, double dist)
{
	x = xCoord; y = yCoord;
	distance = dist;
}

void Node::draw(int size, bool filled, bool showLabel)
{
	// need to adjust size of diamond to account for scaling
	double halfSize = size * sqrt(2.);
	float transMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, transMatrix);
	// transMatrix  0: x-scale, 5: y-scale, 12: x-trans, 13: y-trans

	halfSize /= transMatrix[0];

	// account for filled or not
	if (filled)
		glBegin(GL_QUADS);
	else
		glBegin(GL_LINE_LOOP);

	// simple diamond 
	glVertex2f(x - halfSize, y);
	glVertex2f(x, y + halfSize);
	glVertex2f(x + halfSize, y);
	glVertex2f(x, y - halfSize);
	glEnd();


	if (showLabel) { // show coords
		string coords = StringPlus::sigFig(x,3) + ", " + StringPlus::sigFig(y,3) 
			+ ", d=" + StringPlus::sigFig(distance,3);
		glRasterPos2i(x + 1 * halfSize, y + 2 * halfSize);
		YsGlDrawFontBitmap6x10(coords.c_str());
	}
}

ostream& operator<<(ostream& os, const Node& aNode)
{
	os << aNode.x << "\t" << aNode.y << "\t" ;
	os << aNode.distance;
	
	return os;
}
