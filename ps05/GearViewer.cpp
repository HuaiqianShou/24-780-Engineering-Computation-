

#include <string.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "yssimplesound.h"
#include "DrawingUtilNG.h"
#include "Gear.h"
#include "GearSystem.h"



//Author: Dr.Nestor Gomez & Huaiqian Shou 
//AndrewID: hshou
//Date:10/16/2021
//Course:24780-B
//Description of the code: Modified from Dr.Nestor Gomez's code. Contains all the functions related to the view system.
//It display gears in a window.

using namespace std;

void showMenu();


Point2D getModelCoords(int screenX, int screenY,
	float originX, float originY, float scale);

int main(void)
{
	const int width = 800, height = 600;
	float originX = 0., originY = 0.;
	float scale = 10.;
	int mouseEvent, leftButton, middleButton, rightButton;
	int screenX, screenY;
	float scaleFactor = 1.07;
	stringstream coordStream; // for displaying coordinates on screen
	Point2D modelCoord;
	GearSystem theSystem;
	bool terminate = false;
	bool rotateIsOn = false;
	int rotateDir = 1;
	float rotateMicro = 1.f;
	char yesNo;
	int currGearIndex = -1;
	Gear newGear;
	vector<Gear*> allGears;
	string datafilename;
	int key;
	ifstream inFile;
	bool canPlaySound = false;


	bool waitingToAttach = false;
	bool waitingToMesh = false;

	FsOpenWindow(16, 16, width, height, 1, "Gear Viewer (NG2021)"); // use double buffer

	YsSoundPlayer player1;
	YsSoundPlayer::SoundData clickSound;
	if (YSOK == clickSound.LoadWav("click.wav")) {
		canPlaySound = true;
		// start the player after opening window
		player1.Start();
	}
	else {
		cout << "Failed to read " << "click.wav" << endl;
			return 1;
	}

	showMenu();

	//Here we add some gears, and users can add other gears in the consol.
	theSystem.addGear("B2.gear");
	theSystem.addGear("B1.gear");
	theSystem.addGear("B3.gear");
	theSystem.addGear("B4.gear");
	theSystem.getOriginAndScale(originX, originY, scale);

	

	while (!terminate) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		FsPollDevice();
		key = FsInkey();

		switch (key) {
		case FSKEY_ESC:
		case FSKEY_X:   // exit program
			terminate = true;
			break;
		case FSKEY_UP: // prev gear
			if (waitingToAttach || waitingToMesh)
				theSystem.changeOther(-1);
			else
				theSystem.changeCurrent(-1);
			break;
		case FSKEY_DOWN: // next gear
			if (waitingToAttach || waitingToMesh)
				theSystem.changeOther(+1);
			else
				theSystem.changeCurrent(+1);
			break;
		case FSKEY_LEFT: // rotate CCW
			rotateDir = 1;
			theSystem.rotateCurrent(rotateDir * 10.);
			if (canPlaySound)
				player1.PlayBackground(clickSound, true);
			break;
		case FSKEY_RIGHT: // rotate CW
			rotateDir = -1;
			theSystem.rotateCurrent(rotateDir * 10.);
			if (canPlaySound)
				player1.PlayBackground(clickSound, true);
			break;
		case FSKEY_L: // list all data and redisplay menu
			cout << "\n\n";
			theSystem.printCurrent();
			showMenu();
			break;
		case FSKEY_R: // add a curve
			rotateIsOn = !rotateIsOn;
			break;
		case FSKEY_E:
			theSystem.editCurrent();
			break;
		case FSKEY_T:
			if (waitingToAttach) {
				theSystem.attachGears();
				waitingToAttach = false;
			}
			else
				waitingToAttach = true;
			break;

		case FSKEY_M:
			if (waitingToMesh) {
				theSystem.meshGears();
				waitingToMesh = false;
			}
			else
				waitingToMesh = true;
			break;
		case FSKEY_A: // add a curve
			theSystem.addGear();
			// redisplay console menu
			showMenu();
			break;
		case FSKEY_D: // delete a gear, if there are any in the collection
			theSystem.deleteCurrent();
			theSystem.getOriginAndScale(originX, originY, scale);
			showMenu();
			break;
		case FSKEY_S:
			theSystem.saveAll();
		}

		//adjust the graphics plane
		glTranslated(0, height, 0); // make bottom-left the origin
		glScaled(scale, -scale, 1.);  // scale everything, with positive y going up
		glTranslated(-originX, -originY, 0);

		if (rotateIsOn)
			theSystem.rotateCurrent(rotateDir * rotateMicro);

		theSystem.draw(waitingToAttach,waitingToMesh);
	
		// reset transformation matrix completely
		glLoadIdentity();
		
		//display helpful message(This is from Dr.Nestor Gomez's code and I copied it during the lecture)
		if (waitingToAttach || waitingToMesh) {
			coordStream.str("");

			if (waitingToAttach)
				coordStream << "Select other gear and press \"T\" to complete attach";
			else
				coordStream << "Select other gear and press \"M\" to complete mesh";
			glColor3ub(255, 255, 255);
			DrawingUtilNG::drawRectangle(0, 0, 370, 20, true);
			glColor3ub(0, 0, 255);
			glRasterPos2i(5, 15);
			YsGlDrawFontBitmap7x10(coordStream.str().c_str());
		}

		mouseEvent = FsGetMouseEvent(leftButton, middleButton,
			rightButton, screenX, screenY);


		if (leftButton) {
			// write coords on screen if left button is held down
			modelCoord = getModelCoords(screenX, screenY,
				originX, originY, scale);
			coordStream.str("");  // reset stream
			coordStream.precision(4);
			coordStream << modelCoord.x << ", " << modelCoord.y;
			glColor3ub(60, 230, 60);
			glRasterPos2i(screenX, screenY);  // set position
			YsGlDrawFontBitmap7x10(coordStream.str().c_str());
		}

		FsSwapBuffers();
		FsSleep(20);
	}

	if (canPlaySound)
		player1.End();

}


void showMenu()
{
	cout << "\n\n";
	cout << "Use these keys on the graphics window:" << endl << endl;
	cout << "\tA : Add a gear from input file" << endl;
	cout << "\tD : Delete the current selected gear from the collection" << endl;
	cout << "\tL : List all the data for the currently selected gear on the console window" << endl;
	cout << endl;
	cout << "\tUp/Down arrows: cycle through gears in the collection" << endl;
	cout << "\tLeft/Right arrows: rotate the currently selected gear" << endl;
	cout << "\tR : Keep rotating currently selected gear (in previous direction)" << endl;
	cout << endl;
	cout << "\tLeft click on gear to make current" << endl;
	cout << "\tLeft click and hold anywhere to see coordinates" << endl;
	cout << "\tE : Edit the current gear"<<endl;
	cout << "\tT : Attach the current gear to another gear"<<endl;
	cout << "\tM : Mesh the current gear into another gear" << endl;
	cout << "\tS : Save all the gears to a single file" << endl;
	cout << endl;
	cout << "\tX/esc : eXit the program" << endl;
}



Point2D getModelCoords(int screenX, int screenY, float originX, float originY, float scale) {
	int wei, hei;
	FsGetWindowSize(wei, hei);
	return { screenX / scale + originX, (hei - screenY) / scale + originY };
}