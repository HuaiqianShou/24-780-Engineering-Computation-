#include "Lshape.h"
#include <sstream>
#include <algorithm>    // std::min
#include <math.h>
#include <fstream>
using namespace std;


void Lshape::readFile(std::ifstream& inFile)
{
	string wholeLineString;
	stringstream wholeLineStream;
	bool continueReading = true;
	int colonLocation;

	// go through file
	while (!inFile.eof() && continueReading) {
		// read the whole line
		getline(inFile, wholeLineString);

		// find the colon and prepare to read from stringstream after the colon
		if ((colonLocation = wholeLineString.find(":")) != string::npos)
			wholeLineStream.str(wholeLineString.substr(colonLocation + 1));

		// when the find() function doesn't find it, string::npos is returned
		//if (wholeLineString.find("partID") != string::npos) {
		//	partID = StringPlus::trim(
		//		wholeLineString.substr(colonLocation + 1));
		//}
		if (wholeLineString.find("locX") != string::npos) {
			wholeLineStream >> locX;
		}
		else if (wholeLineString.find("locY") != string::npos) {
			wholeLineStream >> locY;
		}
		else if (wholeLineString.find("height") != string::npos) {
			wholeLineStream >> height;
		}
		else if (wholeLineString.find("width") != string::npos) {
			wholeLineStream >> width;
		}
		else if (wholeLineString.find("thickness") != string::npos) {
			wholeLineStream >> thickness;
		}
		else if (wholeLineString.find("theta") != string::npos) {
			wholeLineStream >> theta;
		}

		wholeLineStream.clear(); // get ready for next line
	}
	
	recalcSpline();

}

void Lshape::writeFile(std::ostream& output)
{

	output << "locX: " << locX << endl;
	output << "locY: " << locY << endl;
	output << "height: " << height << endl;
	output << "width: " << width << endl;
	output << "thickness: " << thickness << endl;
	output << "theta: " << theta << endl;


}

void Lshape::recalcSpline()
{
	// put a bunch of nodes into theGuides
	theGuides.clear();
	
	theGuides.push_back(Node(locX, locY));
	theGuides.push_back(Node(locX + width, locY));
	theGuides.push_back(Node(locX + width, locY + thickness));
	theGuides.push_back(Node(locX + thickness, locY + thickness));
	theGuides.push_back(Node(locX + thickness, locY + height));
	theGuides.push_back(Node(locX, locY + height));
	

	Shape::recalcSpline();
}
