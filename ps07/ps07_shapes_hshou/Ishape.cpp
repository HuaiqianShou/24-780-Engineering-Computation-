#include "Ishape.h"
#include <sstream>
#include <algorithm>    // std::min
#include <math.h>
#include <fstream>
#include<iostream>
using namespace std;
void Ishape::readFile(std::ifstream& inFile)
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
		else if (wholeLineString.find("flangeT") != string::npos) {
			wholeLineStream >> flangeT;
		}
		else if (wholeLineString.find("webT") != string::npos) {
			wholeLineStream >> webT;
		}
		else if (wholeLineString.find("theta") != string::npos) {
			wholeLineStream >> theta;
		}

		wholeLineStream.clear(); // get ready for next line
	}

	recalcSpline();



}

void Ishape::writeFile(std::ostream& output)
{
	output << "locX: " << locX << endl;
	output << "locY: " << locY << endl;
	output << "height: " << height << endl;
	output << "width: " << width << endl;
	output << "flangeT: " << flangeT << endl;
	output << "webT: " << webT << endl;
	output << "theta: " << theta << endl;

}

void Ishape::recalcSpline()
{


	// put a bunch of nodes into theGuides
	theGuides.clear();

	theGuides.push_back(Node(locX - width / 2., locY - height / 2.));
	theGuides.push_back(Node(locX + width / 2., locY - height / 2.));
	theGuides.push_back(Node(locX + width / 2., locY - height / 2. + flangeT));
	//using namespace std;
	//cout << "hello!" << webT << endl;
	//cout << webT / 2.00 << endl;
	theGuides.push_back(Node(locX + webT / 2., locY - height / 2. + flangeT));
	theGuides.push_back(Node(locX + webT / 2., locY + height / 2. - flangeT));
	theGuides.push_back(Node(locX + width / 2., locY + height / 2. - flangeT));

	theGuides.push_back(Node(locX + width / 2., locY + height / 2.));
	theGuides.push_back(Node(locX - width / 2., locY + height / 2.));

	theGuides.push_back(Node(locX - width / 2., locY + height / 2. - flangeT));
	theGuides.push_back(Node(locX - webT / 2., locY + height / 2. - flangeT));
	theGuides.push_back(Node(locX - webT / 2., locY - height / 2. + flangeT));
	theGuides.push_back(Node(locX - width / 2., locY - height / 2. + flangeT));
	//theGuides.push_back(Node(locX, locY + height));


	Shape::recalcSpline();
}
