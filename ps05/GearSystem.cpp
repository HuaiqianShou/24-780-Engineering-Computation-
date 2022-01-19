#include "GearSystem.h"
#include <sstream>
#include <algorithm>    // std::min
#include <math.h>
#include "fssimplewindow.h"
#include "StringPlus.h"
#include "Gear.h"
#include <iostream>
#include <string>
#include <fstream>

char yesNo;

using namespace std;



//Author: Huaiqian Shou 
//AndrewID: hshou
//Date:10/16/2021
//Course:24780-B
//Description of the code: Modified from some of Dr.Nestor Gomez's code. It is a class of gearsystem.
//It creates a system that contains all the gears and contains all the functions related to the gearsystem

void GearSystem::draw(bool waitingToAttach,bool waitingToMesh, bool highlightOther)
{
	if (!(waitingToMesh||waitingToAttach)) {
		glColor3ub(0, 0, 0);
		glLineWidth(1);
		for (Gear* currGear : allGears)
			currGear->draw(highlightOther);
		glColor3ub(255, 0, 0);
		glLineWidth(3);
		if (currGearIndex >= 0 && currGearIndex < int(allGears.size())) {// extra protection
			allGears.at(currGearIndex)->draw(highlightOther);
		}
	}
	//else if (waitingToAttach) {
	//	glColor3ub(0, 0, 0);
	//	glLineWidth(1);
	//	for (Gear* currGear : allGears)
	//		currGear->draw(highlightOther);
	//	glColor3ub(255, 0, 0);
	//	glLineWidth(3);
	//	if (currGearIndex >= 0 && currGearIndex < int(allGears.size())) {// extra protection
	//		allGears.at(currGearIndex)->draw(highlightOther);
	//	}
	//}
	else {
		glColor3ub(0, 0, 0);
		glLineWidth(1);
		for (Gear* currGear : allGears)
			currGear->draw(highlightOther);
		glColor3ub(255, 0, 0);
		glLineWidth(3);
		if (otherGearIndex >= 0 && otherGearIndex < int(allGears.size())) {// extra protection
			allGears.at(otherGearIndex)->draw(highlightOther);
		}
	}



}


void GearSystem::rotateCurrent(float deg)
{	
	float rotate_angle = 360. / allGears.at(currGearIndex)->getNumbTeeth();
	/*printf("jiaodu: %fl", rotate_angle);*/
	allGears.at(currGearIndex)->rotate(deg);
}



void GearSystem::deleteCurrent()
{
	if (currGearIndex >= 0) {
		// redirect to console
		inputRequiredMessage();

		// ask for verification
		cout << "\n\nAre you sure you want to delete this gear ("
			<< allGears.at(currGearIndex)->getID() << ")? (Y/N) >> ";
		cin >> yesNo;

		// delete/erase the current curve and make the previous curve current
		if (yesNo == 'Y' || yesNo == 'y') {
			allGears.erase(allGears.begin() + currGearIndex);
			currGearIndex--;
			if (currGearIndex < 0)
				currGearIndex = int(allGears.size()) - 1;
			//getOriginAndScale(originX, originY, scale);
		}


	}
}

void GearSystem::editCurrent()
{
	allGears.at(currGearIndex)->edit();
}

void GearSystem::printCurrent()
{
	allGears.at(currGearIndex)->print();
}

void GearSystem::changeCurrent(int change)
{	
	int numGear = int(allGears.size());
	if (currGearIndex + change == numGear) {
		currGearIndex = 0;
	}
	else if (currGearIndex + change == -1) {
		currGearIndex = numGear - 1;
	}
	else {
		currGearIndex = currGearIndex + change;
	}
		
}

void GearSystem::changeOther(int change)
{
	int numGear = int(allGears.size());
	if (otherGearIndex + change == numGear) {
		otherGearIndex = 0;
	}
	else if (otherGearIndex + change == -1) {
		otherGearIndex = numGear - 1;
	}
	else {
		otherGearIndex = otherGearIndex + change;
	}
}

void GearSystem::changeCurrent(Point2D givenCoords)
{

	int i = 0;
	while (i < int(allGears.size()) && !(allGears.at(i)->pointIsInGear(givenCoords)));
		i++;
	if (i < int(allGears.size())) // found a gear under mouse
		currGearIndex = i;
}



void GearSystem::getOriginAndScale(float& originX, float& originY, float& scale)
{

	if (int(allGears.size() > 0)) {
		Point2D minBnd, maxBnd, currMin, currMax;

		minBnd = allGears[0]->minBound();
		maxBnd = allGears[0]->maxBound();
		for (Gear* currGear : allGears) {
			currMin = currGear->minBound();
			currMax = currGear->maxBound();
			if (minBnd.x > currMin.x)
				minBnd.x = currMin.x;
			if (minBnd.y > currMin.y)
				minBnd.y = currMin.y;
			if (maxBnd.x < currMax.x)
				maxBnd.x = currMax.x;
			if (maxBnd.y < currMax.y)
				maxBnd.y = currMax.y;
		}
		originX = minBnd.x; originY = minBnd.y;

		int hei, wid;
		FsGetWindowSize(wid, hei);

		float scaleX, scaleY;
		int extraBorder = 10; // allows for some space around all the gears
		scaleX = (wid - extraBorder) / (maxBnd.x - minBnd.x);
		scaleY = (hei - extraBorder) / (maxBnd.y - minBnd.y);
		scale = min(scaleX, scaleY);
		if (scale == scaleX) // adjust originY so as to center vertically
			originY -= 0.5 * (hei / scale - (maxBnd.y - minBnd.y));
		else // adjust originX so as to center horizontally
			originX -= 0.5 * (wid / scale - (maxBnd.x - minBnd.x));

		// adjust for half extraborder
		originX -= extraBorder / 2. / scale;
		originY -= extraBorder / 2. / scale;
	}
	else {
		originX = originY = 0.;
		scale = 1.0;
	}
}

bool GearSystem::attachGears()
{

	allGears.at(currGearIndex)->attachToGear(allGears.at(otherGearIndex));
	return true;
}

bool GearSystem::meshGears()
{
	allGears.at(currGearIndex)->meshInto(allGears.at(otherGearIndex));
	return true;
}

void GearSystem::saveAll()
{

	ofstream outFile;
	string datafilename;

	//ask for filename
	cout << "\n\nPlease enter the name of the file to save>";
	cin >> datafilename;

	if (datafilename.find(".gear") == string::npos)
		datafilename += ".gear";
	outFile.open(datafilename);

	if (outFile.is_open()) {
		for (auto currGear : allGears) {
			outFile << *currGear;
		}
		outFile.close();
	}
	else
		cout << "\nError writing file. Please check data and try again." << endl;
}

void GearSystem::changeOther(Point2D givenCoords)
{
	int i = 0;
	while (i < int(allGears.size()) && !(allGears.at(i)->pointIsInGear(givenCoords)));
	i++;
	if (i < int(allGears.size())) // found a gear under mouse
		otherGearIndex = i;
}

void GearSystem::addGear(const std::string filename)
{
	ifstream inFile;
	string datafilename;

	if (filename == "") {

		inputRequiredMessage();
		cout << "\n\nPlease enter the name of the file to read" << endl;
		cin >> datafilename;
	}
	else
		datafilename = filename;
	if (datafilename.find(".gear") == string::npos)
		datafilename += ".gear";
	inFile.open(datafilename);

	if (inFile.is_open()) {
		string wholeLineString;

		while (!inFile.eof()) {
			getline(inFile, wholeLineString);
			if (wholeLineString.find("Gear:") != string::npos) {
				Gear* newGear = new Gear;
				newGear->load(inFile);
				allGears.push_back(newGear);
			}
		}
		inFile.close();
		currGearIndex = int(allGears.size()) - 1;
	}
	else
		cout << "\nError reading file.Please check data and try again" << endl;
}

Gear* GearSystem::getGear(const std::string& gearID)
{
	string wantedID = StringPlus::trim(gearID);
	Gear* wantedGear = nullptr;
	for (auto currGear : allGears) {
		if (currGear->getID() == wantedID) {
			wantedGear = currGear;
			break;
		}
	}

	return wantedGear;
}
