/*
Nestor Gomez
Carnegie Mellon University
Eng. Computation, 24-780-B
Prob Set 6
Due Tues. Oct. 26, 2021
*/
#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#include "Shape.h"
#include "yssimplesound.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//using namespace std;  // removed so that other files are not forced into it
class Node;
//class Slider;

class ViewManager {
	// this enum only exists in the context of ViewManager
	enum UserMode { none, editMode, addMode, deleteMode };

private:
	Shape *theShape;  // may change to many shapes later

	// simulation parameters
	bool simulationIsRunning;

	std::chrono::system_clock::time_point startTime; 
	// above can be set like this:  startTime = std::chrono::system_clock::now();
	// and then can do:             
	//    std::chrono::duration<double> elapsedTime = std::chrono::system_clock::now() - startTime;
	// which can then be used to get elapsed seconds:  elapsedSeconds = elapsedTime.count();

	// view preferences
	int panChange;
	double zoomFactor;

	int shapeColor;  // H value for shape color (S = 1, V = 1)
	int nodeColor;  // H value for node color (S = 1, V = 1)
	int lineWidth;
	bool showNodes;
	bool showAxes;
	bool showProperties;

	int xOrigin, yOrigin;  // screen coords of model coords 0,0
	int prevLocX, prevLocY;  // for zoom and pan, and for node edit
	double viewScale;  // must be greater than zero
	double splineTension = 0.;

	// to help with file names
	std::vector<std::string> allShapeFiles;

	// to manage add/edit
	UserMode currUserMode;
	Node * currNode;

	// manage undo of guide point edit
	Node* lastNode;
	double lastNodeX, lastNodeY;

	// audio feedback on inside/outside check
	YsSoundPlayer theSoundPlayer;
	YsSoundPlayer::SoundData insideSound;
	YsSoundPlayer::SoundData outsideSound;

public:
	ViewManager();

	// displays keycode choices on console
	void showMenu(); 
	
	int getOriginX() { return xOrigin; }
	int getOriginY() { return yOrigin; }
	double getScale() { return viewScale; }

	// interprets user input and performs actions on the interface
	// returns false only when user presses ESC to end program
	bool manage();

	// given model coordinates, function calculates screen coordinates
	// converting for translation and scale
	void getScreenCoords(double modelX, double modelY,
		double& screenX, double& screenY);

	// given screen coordinates, function calculates model coordinates
	// converting for translation and scale
	void getModelCoords(double &modelX, double &modelY,
		double screenX, double screenY);

	// given model coordinates, function adds a vertex on screen
	// after converting for translation and scale
	void screenVertex(double modelX, double modelY);

private:
	// asks for a filename and loads a file into model
	void load();

	// asks for a filename and loads a file into model
	void save();

	// sets view parameters so that the model is centered on screen
	void centerOnScreen();

	// allows user to say Y/N on screen
	bool getConsoleYes();   
	
	// allows user to say Y/N on console
	bool getScreenYes();
	
	// allows user to input file name in console
	std::string getFileFromConsole();  
	
	// allows user to input file name on screen, with a list of available files
	// requires using C++17 compiler (set in project properties)
	std::string getFileFromScreen(std::vector<std::string>& availableFiles,
		const std::string& prompt);

	void getAvailableFiles(std::vector<std::string>& availableFiles);

	void drawModeIndicator();

	void highlightNode(Node& aNode);

};