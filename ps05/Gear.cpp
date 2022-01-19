#include <sstream>
#include <algorithm>    // std::min
#include <math.h>
#include "fssimplewindow.h"
#include "StringPlus.h"
#include "Gear.h"
#ifndef M_PI
#define M_PI 3.14159265
#endif
using namespace std;


//Author: Huaiqian Shou 
//AndrewID: hshou
//Date:10/16/2021
//Course:24780-B
//Description of the code: Modified from some of Dr.Nestor Gomez's code. Contains all the functions related to the gear


void Gear::load(std::ifstream& inFile)
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
		if (wholeLineString.find("partID") != string::npos) {
			partID = StringPlus::trim(
				wholeLineString.substr(colonLocation + 1));
		}
		else if (wholeLineString.find("pitch") != string::npos) {
			wholeLineStream >> pitch;
		}
		else if (wholeLineString.find("numbTeeth") != string::npos) {
			wholeLineStream >> numbTeeth;
		}
		else if (wholeLineString.find("startX") != string::npos) {
			wholeLineStream >> loc.x;
		}
		else if (wholeLineString.find("startY") != string::npos) {
			wholeLineStream >> loc.y;
		}
		else if (wholeLineString.find("startAngle") != string::npos) {
			wholeLineStream >> angle;
		}

		else if (wholeLineString.find("Gear End") != string::npos) {
			continueReading = false;
		}

		wholeLineStream.clear(); // get ready for next line
	}
}

void Gear::print(std::ostream& output)
{
	output << "Gear: " << endl;
	output << "\tpartID: " << partID << endl;
	output << "\tpitch: " << pitch << endl;
	output << "\tnumbTeeth: " << numbTeeth << endl;
	output << "\tstartX: " << loc.x << endl;
	output << "\tstartY: " << loc.y << endl;
	output << "\tstartAngle: " << angle << endl;
	output << "Gear End: " << endl;
}

void Gear::draw(bool fill)
{
	float radius = numbTeeth / pitch / 2.;
	float module = radius * 2 / numbTeeth;
	float angleStepRad = 360. / numbTeeth * atan(1.) / 45.;

	glTranslated(loc.x, loc.y, 0.);
	glRotated(angle, 0., 0., 1.);
	float topWidth = 1.0 * module;
	float botWidth = 1.8 * module;
	float radiusToToothSide = sqrt(pow(radius - 1.25 * module, 2.) + pow(botWidth / 2., 2.));
	float theta = angleStepRad - atan(botWidth / 2. / (radius - 1.25 * module));

	// draw the teeth
	for (int i = 0; i < numbTeeth; i++) {

		if (!fill) {
			glBegin(GL_LINE_STRIP);
			glVertex2d(radiusToToothSide * cos(theta), radiusToToothSide * sin(theta));
			glVertex2d(radius - 1.25 * module, botWidth / 2.);
			glVertex2d(radius + 1.00 * module, topWidth / 2.);
			glVertex2d(radius + 1.00 * module, -topWidth / 2.);
			glVertex2d(radius - 1.25 * module, -botWidth / 2.);
			glEnd();
		}
		else {
			// filled in (two convex shapes)
			glBegin(GL_POLYGON);
			glVertex2d(0., 0.);
			glVertex2d(radius - 1.25 * module, botWidth / 2.);
			glVertex2d(radius + 1.00 * module, topWidth / 2.);
			glVertex2d(radius + 1.00 * module, -topWidth / 2.);
			glVertex2d(radius - 1.25 * module, -botWidth / 2.);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex2d(0., 0.);
			glVertex2d(radiusToToothSide * cos(theta), radiusToToothSide * sin(theta));
			glVertex2d(radius - 1.25 * module, botWidth / 2.);
			glEnd();
		}
		glRotated(360. / numbTeeth, 0., 0., 1.);
	}

	glRotated(-angle, 0., 0., 1.);
	glTranslated(-loc.x, -loc.y, 0.);

	// draw pitch circle
	//float PI = atan(1.) * 4.;
	//glBegin(GL_LINE_LOOP);
	//for (float angle = 0; angle < 2* PI; angle += PI/32) // 64 points
	//	glVertex2d(loc.x + radius* cos(angle), loc.y + radius * sin(angle));
	//glEnd();

}

void Gear::rotate(float rotAngle, Gear* requestingGear)
{
	angle += rotAngle;
	angle = fmod(angle, 360.);
	if (requestingGear == nullptr) {
		for (auto currGear : attachedGears) {
			if (currGear != requestingGear)
				currGear->rotate(rotAngle, this);


		}
		for (auto currGear : meshedGears) {
			if (currGear != requestingGear) {
				float otherAngle = rotAngle * numbTeeth / currGear->getNumbTeeth();
				currGear->rotate(otherAngle, this);

			}

		}


	}
}

Point2D Gear::minBound()
{
	// should actually use addendum dimension
	float radius = numbTeeth / pitch / 2.;
	float module = radius * 2 / numbTeeth;
	return { loc.x - radius - module, loc.y - radius - module };
}
Point2D Gear::maxBound()
{
	// should actually use addendum dimension
	float radius = numbTeeth / pitch / 2.;
	float module = radius * 2 / numbTeeth;
	return { loc.x + radius + module, loc.y + radius + module };
}

bool Gear::attachToGear(Gear* otherGear, bool biDirectional)
{
	attachedGears.push_back(otherGear);
	// move loc.x and loc.y to match otherGear
	loc = otherGear->getLoc();

	if (biDirectional)
		otherGear->attachToGear(this, false);
	return true;
}


void Gear::edit()
{

	string tempString;

	cout << "\nFor each of the following, enter \".\" to keep original value:" << endl;

	cout << "\tpartID (" << partID << ") new value >> ";
	cin >> tempString;
	StringPlus::trim(tempString);
	if (tempString != ".")
		partID = tempString;

	cout << "\tpitch (" << pitch << ") new value >> ";
	cin >> tempString;
	StringPlus::trim(tempString);
	if (tempString != ".")
		pitch = stof(tempString);

	cout << "\tnumbTeeth (" << numbTeeth << ") new value >> ";
	cin >> tempString;
	StringPlus::trim(tempString);
	if (tempString != ".")
		numbTeeth = stoi(tempString);

	cout << "\tstartX (" << loc.x << ") new value >> ";
	cin >> tempString;
	StringPlus::trim(tempString);
	if (tempString != ".")
		loc.x = stof(tempString);

	cout << "\tstartY (" << loc.y << ") new value >> ";
	cin >> tempString;
	if (tempString != ".")
		loc.y = stof(tempString);

	cout << "\tangle (" << angle << ") new value >> ";
	cin >> tempString;
	if (tempString != ".")
		angle = stof(tempString);
}


bool Gear::meshInto(Gear* otherGear, bool biDirectional)
{	
	meshedGears.push_back(otherGear);

	float curr_x = loc.x;
	float curr_y = loc.y;
	
	float other_x = otherGear->getLoc().x;
	float other_y = otherGear->getLoc().y;

	float deltaX = curr_x - other_x;
	float deltaY = curr_y - other_y;

	float curr_dist = sqrt(deltaX * deltaX + deltaY * deltaY);
	float newDist = pitchDiam() / 2 + otherGear->pitchDiam() / 2;
	float other_angle = otherGear->getAngle();
	float curr_angle = this->getAngle();

	loc.x = other_x + deltaX * newDist / curr_dist;
	loc.y = other_y + deltaY * newDist / curr_dist;

	float angle_line = atan2(deltaY,deltaX)*180./M_PI;
	float anglg_to_rotate = 180 - this->getAngle() + angle_line + 180./this->getNumbTeeth();
	//cout << "this angle:"<<this->getAngle() << endl;
	//cout << "angle_line:" << angle_line << endl;
	//cout << "teeth angle:" << 180. / this->getNumbTeeth() << endl;
	//cout << "angle to rotate:" << anglg_to_rotate << endl;
	this->rotate(anglg_to_rotate);
	otherGear->rotate(-other_angle);
	if (biDirectional)
		otherGear->meshInto(this, false);
	return true;

}


std::ostream& operator<<(std::ostream& out, const Gear& aGear)
{
	out << "Gear:" << endl;
	out << "\tpartID:" << aGear.partID << endl;
	out << "\tpitch:" << aGear.pitch << endl;
	out << "\tnumbTeeth:" << aGear.numbTeeth << endl;
	out << "\tstartX:" << aGear.loc.x << endl;
	out << "\tstartY:" << aGear.loc.y << endl;
	out << "\tstartAngle: " << aGear.angle << endl;
	out << "Gear End: " << endl;
	return out;
}