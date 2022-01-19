#pragma once
#include <string.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "yssimplesound.h"
#include "Gear.h"


using namespace std;

//Author: Huaiqian Shou 
//AndrewID: hshou
//Date:10/16/2021
//Course:24780-B
//Description of the code: Modified from some of Dr.Nestor Gomez's code. It is header file of gearsystem.


class GearSystem {
private:
	std::vector<Gear*> allGears;
	int currGearIndex;
	int otherGearIndex;

public:
	// draws all gears, highlighting current gear and optionally
	// highlighting otherGear.
	void draw(bool waitingToAttach, bool waitingToMesh, bool highlightOther = false);

	// displays helpful message on graphics window
	void inputRequiredMessage() {
		// display message on graphics window directing user to check console

		string message = "Input required on console . . .";
		// black color
		glColor3f(0, 0, 0);

		//center horizontally and vertically for 20-pixel-wide by 28-pixel-height letters
		int wid, hei;
		FsGetWindowSize(wid, hei);
		glRasterPos2d(wid / 2 - message.length() * 20 / 2, hei / 2 + 28 / 2);

		YsGlDrawFontBitmap20x28(message.c_str()); // function requires a C-language string
		FsSwapBuffers();

	}



	// rotates the current gear
	void rotateCurrent(float deg);

	// removes current gear, with confirmation
	void deleteCurrent();

	// edits current gear
	void editCurrent();

	// prints current gear to console
	void printCurrent();

	// changes the current gear, being careful to "loop around " as needed
	void changeCurrent(int change);

	// changes the other gear, being careful to "loop around " as needed
	// but skipping over current gear
	void changeOther(int change);

	// OPTIONAL:
	// changes the current gear based on given model coords
	void changeCurrent(Point2D givenCoords);

	// OPTIONAL:
	// changes the other gear based on given model coords
	// but skipping over current gear
	void changeOther(Point2D givenCoords);

	// asks for filename and loads a gear or gears,
	// adding it/them to system
	void addGear(const std::string filename = "");

	// returns a pointer to the gear that matches the givenID,
	// returns nullptr if not found
	Gear* getGear(const std::string& gearID);

	// returns a pointer to the gear at given index,
	// returns nullptr if not found
	Gear* getGear(int gearIndex) {
		if (0 <= gearIndex && gearIndex < allGears.size())
			return allGears.at(gearIndex);
		else
			return nullptr;
	}

	// figures out best origin and scale for display
	void getOriginAndScale(float& originX, float& originY, float& scale);

	// attaches the current gear to the other gear
	bool attachGears();

	// meshes the current gear into the other gear
	bool meshGears();

	// asks for a file name and saves all gears in a single file
	void saveAll();
};

