
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

//Author: Dr.Nestor Gomez& Huaiqian Shou 
//AndrewID: hshou
//Date:10/16/2021
//Course:24780-B
//Description of the code: Modified from Dr.Nestor Gomez's code. Header file for the gear class


struct Point2D {
	float x, y;
};

class Gear {
private:
	std::string partID; // used for identification
	float pitch;        // given in teeth per inch
	int numbTeeth;
	Point2D loc;        // current location of gear {x, y}
	float angle;        // current rotation of gear in degrees
	std::vector<Gear*> attachedGears;
	std::vector<Gear*> meshedGears;

public:

	Gear() {
		partID = "unassigned";
		pitch = 0;
		numbTeeth = 0;
		loc = { 0,0 };
		angle = 0.;
	}

	// reads data from a file over-riding any existing data

	void load(std::ifstream& inFile);

	// outputs all the data for a gear in format matching attached example
	void print(std::ostream& output = std::cout);

	// calculates and return the pitch diameter
	float pitchDiam() { return numbTeeth / pitch; };

	// draws the gear on graphics window
	void draw(bool fill = false);

	std::string getID() { return partID; }
	int getNumbTeeth() { return numbTeeth; }
	float getPitch() { return pitch; }
	Point2D getLoc() { return loc; }
	float getAngle() { return angle; }


	//void rotate(float rotAngle) { angle += rotAngle; }
	//void rotate(float rotAngle) { angle = std::fmod(angle+rotAngle, 360.); }
	void rotate(float rotAngle, Gear* requestingGear = nullptr);
	
	// returns bottom left corner of gear bounding box
	Point2D minBound();

	// returns top right corner of gear bounding box
	Point2D maxBound();

	bool pointIsInGear(Point2D aPoint) {

		return ((aPoint.x - loc.x) * (aPoint.x - loc.x) + (aPoint.y - loc.y) * (aPoint.y - loc.y)) < (pitchDiam() * pitchDiam())/4.;
	
	}

	// create a bi-directional attach relationship between this gear and
	// otherGear, unless bi-directional is not appropriate
	// may require this gear’s location to change to match otherGear’s
	// returns false if connection cannot be done
	bool attachToGear(Gear* otherGear, bool biDirectional = true);
	
	void edit();
	
	// create a bi-directional meshed relationship between this gear and
	// otherGear, unless bi-directional is not appropriate
	// may require this gear to translate (along line connecting centers)
   // and rotate to allow meshing, but do not move or rotate otherGear
	// returns false if connection cannot be done (unmatched pitches)
	bool meshInto(Gear* otherGear, bool biDirectional = true);

	friend std::ostream& operator<<(std::ostream& out, const Gear& aGear);


};