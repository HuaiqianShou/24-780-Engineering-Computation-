#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>  // only helps when using C++17
//#include <windows.h>   // used to start other programs

#include "fssimplewindow.h"
#include "ViewManager.h"

#include "ysglfontdata.h"
#include "Shape.h"
#include "Node.h"
#include "DrawingUtilNG.h"
#include "StringPlus.h"

using namespace std;

ViewManager::ViewManager()
{
	viewScale = 1.0;
	panChange = 10;
	zoomFactor = 1.1;

	xOrigin = 0;
	yOrigin = WIN_HEIGHT;

	nodeColor = 240;   // blue
	shapeColor = 30;   // reddish
	lineWidth = 2;

	showNodes = false;
	showAxes = false;

	// get available .fem files (also called when a file is saved)
	getAvailableFiles(allShapeFiles);

	currUserMode = none;

	// load sounds and start player
	if (YSOK != insideSound.LoadWav("cash_register.wav"))
		cout << "   ERROR: Unable to load cash_register.wav " << endl;
	if (YSOK != outsideSound.LoadWav("buzzer.wav"))
		cout << "   ERROR: Unable to load cash_register.wav " << endl;
	theSoundPlayer.Start();
}

void ViewManager::load()
{
	string inFileName;
	ifstream inFile;
	//inFileName = getFileFromConsole();
	inFileName = getFileFromScreen(allShapeFiles,
		"Enter file name of model to load.");

	// if user forgets extension, just add it in
	if (inFileName.find(".shape") == string::npos)
		inFileName += ".shape";

	inFile.open(inFileName);

	if (inFile.is_open()) {
		theShape.readFile(inFile);
		inFile.close();
	}
	else
		cout << "Was not able to open " << inFileName << " for input. " << endl;

	// set starting view params
	centerOnScreen();
}

void ViewManager::save()
{
	string outFileName;
	ofstream outFile;
	//outFileName = getFileFromConsole();
	outFileName = getFileFromScreen(allShapeFiles,
		"Enter file name to save the model.");

	// if user forgets extension, just add it in
	if (outFileName.find(".shape") == string::npos)
		outFileName += ".shape";

	outFile.open(outFileName);

	if (outFile.is_open()) {
		theShape.writeFile(outFile);
		outFile.close();
		getAvailableFiles(allShapeFiles); // reset list
	}
	else
		cout << "Was not able to open " << outFileName << " for output. " << endl;


}

void ViewManager::centerOnScreen()
{
	double minX, maxX, minY, maxY;

	theShape.getBounds(minX, maxX, minY, maxY);
	float scaleX = WIN_WIDTH / (maxX - minX);
	float scaleY = WIN_HEIGHT / (maxY - minY);
	viewScale = min(scaleX, scaleY) * 0.95;   // leaves a little bit of white space all around
	xOrigin = WIN_WIDTH / 2 - viewScale * (maxX + minX) / 2;
	yOrigin = WIN_HEIGHT / 2 + viewScale * (maxY + minY) / 2;
}

bool ViewManager::manage()
{
	int key;
	string inFileName;
	ifstream inFile;
	bool nodeIsMoving = false;

	int mouseEvent, leftButton, middleButton, rightButton;
	int locX, locY;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, locX, locY);

	//	if (mouseEvent != FSMOUSEEVENT_NONE) { // needed ?
	if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN || mouseEvent == FSMOUSEEVENT_MBUTTONDOWN) {
		prevLocX = locX; prevLocY = locY;  // capture location of first button press
		if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN && currNode != nullptr) { // prepare for undo move
			lastNodeX = currNode->getX();
			lastNodeY = currNode->getY();
			lastNode = currNode;
		}
	}

	if (middleButton || (FsGetKeyState(FSKEY_CTRL) && leftButton)) { // pan in x and y axes
		// no need for scale since the screen-to-model ratio is not applicable
		// however, I admit I arrived at this through trial and error
		// first I multiplied by viewScale, then I divided, then I did neither
		xOrigin += (locX - prevLocX);
		yOrigin += (locY - prevLocY);
		prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	}

	else if (key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN // these also are triggered by touchpad pinch and two finger scroll
		|| (FsGetKeyState(FSKEY_SHIFT) && leftButton)) { // zoom in and out
		double oldScale = viewScale;
		if (key == FSKEY_WHEELUP || locY < prevLocY)
			viewScale *= (zoomFactor - 1) * 0.4 + 1.0; // less jumpty than zooming with +/- keys
		else if (key == FSKEY_WHEELDOWN || locY > prevLocY)
			viewScale /= (zoomFactor - 1) * 0.4 + 1.0;

		// adjust xOrigin and yOrigin so point under mouse does not move
		// i.e., we can zoom in/out on a specific point
		// a bit complicated since you have to convert old origin to screen coords
		// then adjust xOrigin and yOrigin, then convert to model coords. 
		// what you see below is the simplified equation after all substitutions
		// rounding reduces "shifting"

		xOrigin = (int)round((locX * (oldScale - viewScale)
			+ xOrigin * viewScale) / oldScale);
		yOrigin = (int)round((locY * (oldScale - viewScale)
			+ yOrigin * viewScale) / oldScale);

		prevLocX = locX; prevLocY = locY; // reset previous values to continue move
	}

	else if (currUserMode == editMode) {
		if (mouseEvent == FSMOUSEEVENT_LBUTTONUP)
			theShape.recalcSpline();

		else if (leftButton && currNode != nullptr) {
			nodeIsMoving = true;  // this will prevent searching for a new node
			double modelX, modelY;
			getModelCoords(modelX, modelY, locX, locY);
			currNode->setXY(modelX, modelY);
		}
	}
	else if (currUserMode == addMode) {
		if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN && currNode != nullptr) {
			int insertIndex = theShape.getIndexFromDistance(currNode->getDistance());
			theShape.insertNode(*currNode, insertIndex);
		}

	}
	else if (currUserMode == none && key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		double modelX, modelY;
		getModelCoords(modelX, modelY, locX, locY);
		if (theShape.isContained(modelX, modelY)) {
			theSoundPlayer.PlayOneShot(insideSound);
		}
		else {
			theSoundPlayer.PlayOneShot(outsideSound);
		}
	}

	switch (key) {
		//  1. Press C on screen to clear all data(ask for confirmation on console)
	case FSKEY_C:
		// confirm on console or screen
		//if (getConsoleYes()) {
		if (getScreenYes()) {
			theShape.clearAll();
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		}
		break;

		//  2. Press L on screen to ask user (in console) for file name to load
	case FSKEY_L: load();
		break;

	case FSKEY_S: save();
		break;

		//  3. Press arrow keys on screen to pan model up / down / left / right
	case FSKEY_UP: yOrigin += panChange;
		break;
	case FSKEY_DOWN: yOrigin -= panChange;
		break;
	case FSKEY_LEFT: xOrigin += panChange;
		break;
	case FSKEY_RIGHT: xOrigin -= panChange;
		break;

		//  4. Press + on screen to zoom into model(make model appear bigger)
	case FSKEY_PLUS: viewScale *= zoomFactor;
		break;

		//  5. Press – on screen to zoom out of model(make model appear smaller)
	case FSKEY_MINUS: viewScale /= zoomFactor;
		break;

		// node format
	case FSKEY_N: showNodes = !showNodes;
		break;

	case FSKEY_B:
		if (currUserMode != none || showNodes) // only change color if nodes are showing
			nodeColor = (nodeColor + 240 / 8) % 240;
		break;

		// show neutral axes
	case FSKEY_X: showAxes = !showAxes;
		break;
	case FSKEY_H:
		theShape.calcProperties();
		theShape.printProperties();
		break;
	case FSKEY_O: lineWidth = min(lineWidth + 1, 10);
		break;

	case FSKEY_I: lineWidth = max(lineWidth - 1, 1);
		break;

		// Press Z on screen to re-center model on screen
	case FSKEY_Z: centerOnScreen();
		break;

	case FSKEY_E:
		if (currUserMode == editMode)
			currUserMode = none;
		else
			currUserMode = editMode;

		currNode == nullptr;
		break;

	case FSKEY_A:
		if (currUserMode == addMode)
			currUserMode = none;
		else
			currUserMode = addMode;

		currNode == nullptr;
		break;

	case FSKEY_DEL:
		if (currUserMode == editMode && currNode != nullptr) {
			theShape.deleteNode(currNode);
			currNode == nullptr;
		}

		break;

	case FSKEY_U:
		if (currUserMode == editMode) {  // undo only works if still in edit mode
			lastNode->setX(lastNodeX);
			lastNode->setY(lastNodeY);
			break;
		}
	case FSKEY_F:
		theShape.toggleFilled();
		break;
	//case FSKEY_R:
	//	splineTension = min(splineTension + 0.1, 2.);
	//	theShape.setSplineParams(20, splineTension);
	//	break;
	//case FSKEY_T:
	//	splineTension = max(splineTension - 0.1, 0.);
	//	theShape.setSplineParams(20, splineTension);
	//	break;
	}

	// actually draw the whole model
	glLoadIdentity();                       
	glTranslatef(xOrigin, yOrigin, 0);
	glScalef(viewScale, -viewScale, 1);

	theShape.draw(shapeColor, lineWidth,
		nodeColor, showNodes || (currUserMode != none), showAxes);

	glLoadIdentity(); 

	if (currUserMode == editMode) {
		drawModeIndicator();

		// figure out if there's a node to highlight
		if (!nodeIsMoving) {  // only look for node if no nodes are "on the move"
			double modelX, modelY;
			getModelCoords(modelX, modelY, locX, locY);
			currNode = theShape.findNode(modelX, modelY, 10 / viewScale);
		}
		if (currNode != nullptr) {
			highlightNode(*currNode);
		}
	}
	else if (currUserMode == addMode) {
		drawModeIndicator();
		double modelX, modelY;
		getModelCoords(modelX, modelY, locX, locY);
		currNode = theShape.findNode(modelX, modelY, 10 / viewScale, true);
		if (currNode != nullptr) {
			// only use spline node if it is NOT a guide node
			if (theShape.findNode(currNode->getX(), currNode->getY(), 5 / viewScale, false) == nullptr) {
				highlightNode(*currNode);
			}
			else
				currNode = nullptr;
		}
	}
	FsSwapBuffers();

	return (key != FSKEY_ESC);
}

void ViewManager::getScreenCoords(double modelX, double modelY, double& screenX, double& screenY)
{
	// given model coordinates, function calculates screen coordinates
	// converting for translation and scale
	screenX = modelX * viewScale + xOrigin;
	screenY = modelY * -viewScale + yOrigin; // takes care of y-axis upwards
}

void ViewManager::getModelCoords(double& modelX, double& modelY, double screenX, double screenY)
{
	// given screen coordinates, function calculates model coordinates
	// converting for translation and scale
	modelX = (screenX - xOrigin) / viewScale;
	modelY = (screenY - yOrigin) / -viewScale;
}

void ViewManager::screenVertex(double modelX, double modelY)
{
	// given model coordinates, function adds a vertex on screen
	// after converting for translation and scale
	double screenX, screenY;

	getScreenCoords(modelX, modelY, screenX, screenY);
	glVertex2d(screenX, screenY);
}

void ViewManager::showMenu()
{
	cout << "\n\n";
	cout << "Use these keys on the screen:" << endl;
	cout << "    L : load file" << endl;
	cout << "    S : save file" << endl;
	cout << "    C : clear all data (with confirmation)" << endl;
	cout << endl;
	cout << "    E : toggle edit mode on/off (allows moving guide nodes)" << endl;
	cout << "         press DEL when guide node is highlighted to delete it" << endl;
	cout << "    U : undo last node move (if still in edit mode)" << endl;
	cout << endl;
	cout << "    A : toggle add mode on/off (allows adding nodes along shape)" << endl;
	cout << "         click on highlighted spline node to insert guide node" << endl;
	cout << endl;
	cout << "    X : toggle neutral axes on/off" << endl;
	cout << "    N : toggle guide nodes on/off" << endl;
	cout << "    B : cycle through 8 distinct node colors (if nodes are showing)" << endl;
	cout << endl;
	cout << "    P : cycle through 8 distinct shape colors" << endl;
	cout << "    O : make shape thicker" << endl;
	cout << "    I : make shape thinner" << endl;
	cout << "    F : make shape filled" << endl;
	cout << "    H : show the properties of current shape" << endl;
	//cout << "    R : make shape more rounded" << endl;
	//cout << "    T : make shape less rounded" << endl;
	cout << endl;
	cout << "Panning and Zooming" << endl;
	cout << "    Use arrow keys on screen to pan model up/down/left/right" << endl;
	cout << "    Use +/- to zoom into (bigger) and out of (smaller), respectively" << endl;
	cout << "    Z : zoom-all so that model is centered" << endl;
	cout << "        CTRL+mouse to pan, SHIFT+mouse to zoom or use mouse wheel" << endl;
	cout << endl;
}

bool ViewManager::getConsoleYes() {
	char userChoice;
	cout << "Are you sure you want to clear all nodes (Y/N) >> ";
	cin >> userChoice;
	showMenu(); // So that it is "fresh"
	return (userChoice == 'Y' || userChoice == 'y');
}

bool ViewManager::getScreenYes()
{
	// ask for confirmation yes or no from the graphics window
	glColor3f(1, 0, 0);
	glRasterPos2d(140, 200);
	YsGlDrawFontBitmap20x28("Are you sure you want to clear");
	glRasterPos2d(140, 235);
	YsGlDrawFontBitmap20x28("all nodes? (Y/N)");
	FsSwapBuffers();

	FsPollDevice();
	int key = FsInkey();
	while (key != FSKEY_N && key != FSKEY_Y) {
		FsSleep(25);
		FsPollDevice();
		key = FsInkey();
	}

	return (key == FSKEY_Y);
}

void ViewManager::getAvailableFiles(vector<string>& availableFiles)
{
	availableFiles.clear();
	// need C++17 to work (set in project properties)
	//     Configuration Properties -> General -> C++ Language Standard
	for (const auto& entry : std::filesystem::directory_iterator(".")) {

		wstring ws(entry.path().c_str());
		string currFileName(ws.begin(), ws.end());

		if (currFileName.find(".shape") != string::npos)
			availableFiles.push_back(currFileName.substr(2));
	}
}

void ViewManager::drawModeIndicator()
{
	glLineWidth(4);
	if (currUserMode == editMode)
		glColor3ub(10, 255, 10);
	else
		glColor3ub(225, 120, 225);

	DrawingUtilNG::drawRectangle(2, 2, WIN_WIDTH - 5, WIN_HEIGHT - 5, false);
	DrawingUtilNG::drawRectangle(0, WIN_HEIGHT, 100, -20, true);

	glLineWidth(1);
	glColor3ub(100, 100, 100);
	glRasterPos2i(10, WIN_HEIGHT - 5);

	if (currUserMode == editMode)
		YsGlDrawFontBitmap8x12("Edit Mode");
	else
		YsGlDrawFontBitmap8x12("Add Mode");
}

void ViewManager::highlightNode(Node& aNode)
{
	glLoadIdentity();
	glTranslatef(xOrigin, yOrigin, 0);
	glScalef(viewScale, -viewScale, 1);

	glLineWidth(3);
	if (currUserMode == editMode) {
		glColor3ub(10, 255, 10);
		aNode.draw(lineWidth + 8, false, true);
	}
	else {
		glColor3ub(225, 120, 225);
		aNode.draw(lineWidth + 3, true, true);
	}
	glLineWidth(1);

	glLoadIdentity();
}

string ViewManager::getFileFromScreen(vector<string>& availableFiles, const string& prompt)
{
	int adjustLetter;
	int key;
	string fileName = "";

	FsPollDevice();
	key = FsInkey();
	while (key != FSKEY_ESC && key != FSKEY_ENTER) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// ask for file name from the graphics window
		glColor3f(1, 0, 0);
		glRasterPos2d(140, 200);
		YsGlDrawFontBitmap16x20(prompt.c_str());
		glRasterPos2d(160, 225);
		YsGlDrawFontBitmap12x16("Press ENTER when done, ESC to cancel.");
		glColor3ub(255, 0, 255);
		DrawingUtilNG::drawRectangle(140, 235, 450, 50, false);

		// show list of available files (need C++17 to work, set in project props)
		glRasterPos2d(440, 330);
		YsGlDrawFontBitmap12x16("Available Files:");
		for (int i = 0; i < availableFiles.size(); i++) {
			glRasterPos2d(460, 350 + i * 20);
			YsGlDrawFontBitmap12x16(availableFiles.at(i).c_str());
		}

		// build filename from keyboard entry, letter by letter
		DrawingUtilNG::buildStringFromFsInkey(key, fileName);

		fileName += "_"; // add an underscore as prompt
		glRasterPos2i(165, 275);  // sets position
		YsGlDrawFontBitmap16x24(fileName.c_str());
		fileName = fileName.substr(0, fileName.length() - 1); // remove underscore

		FsSwapBuffers();
		FsSleep(25);

		FsPollDevice();
		key = FsInkey();
	}

	if (key == FSKEY_ENTER) {
		glColor3f(1, 0, 0);
		glRasterPos2d(140, 400);
		YsGlDrawFontBitmap16x20("Loading . . .");

		FsSwapBuffers(); // this keeps the other stuff on because the previous buffer had it too
		return fileName;
	}
	else
		return "";

}

string ViewManager::getFileFromConsole()
{
	glColor3f(0, 0, 0);
	glRasterPos2d(150, 200);
	YsGlDrawFontBitmap20x28("Input required on console . . .");
	FsSwapBuffers();

	string longInput;
	cout << endl << "            Name of file to load (.fem) >> ";
	getline(cin, longInput);

	showMenu(); // So that it is "fresh"

	return StringPlus::trim(longInput);
}
