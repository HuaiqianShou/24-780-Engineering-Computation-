#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>  
#include <conio.h>
#include "fssimplewindow.h"

#pragma warning(disable:4996)

//Author: Huaiqian Shou 
//AndrewID: hshou
//Date:9/21/2021
//Course:24780-B
//Description of the code: Build on that success by generating a graphical view of the data and the resulting best - fit curve and a simple UI

const char* inputfile[3] = { "SpringData01.txt", "SpringData02.txt",
							 "SpringData03.txt" };
using namespace std;
using std::vector;
double x = 0, y = 0;
int colorint = 1;
int fileint = 0;

struct DataPoint {
	double x_point;
	double y_point;
};

enum Color { RED, ORANGE, GREEN, CYAN, BLUE };

struct SpringCurve {
	Color co = RED;
	vector<double> dataX;
	vector<double> dataY;
	double a1 = 0;
	double a2 = 0;
	double max_X = 0;
	double max_Y = 0;
};


void createCurve(const string &inputFile, SpringCurve &aCurve) {
	using std::cout;
	using std::endl;
	double x_sum = 0, y_sum = 0, x2_sum = 0, x3_sum = 0, xy_sum = 0;
	ifstream file(inputFile);
	if (file.is_open()) {
		while (file >> x >> y) {
			x_sum += x;
			y_sum += y;
			x2_sum += pow(x, 2);
			x3_sum += pow(x, 3);
			xy_sum += x * y;
			aCurve.dataX.push_back(x);
			aCurve.dataY.push_back(y);
		}
	}
	//here are the equations to calculate a1 and a2
	aCurve.a2 = (x_sum * xy_sum - x2_sum * y_sum) / (x_sum * x3_sum - x2_sum * x2_sum);
	aCurve.a1 = (-x2_sum * xy_sum + x3_sum * y_sum) / (x_sum * x3_sum - x2_sum * x2_sum);
	using std::max_element;
	//here is to find the max value of a set
	aCurve.max_X = *max_element(aCurve.dataX.begin(), aCurve.dataX.end());
	aCurve.max_Y = *max_element(aCurve.dataY.begin(), aCurve.dataY.end());
	
}


void printCurve(const SpringCurve &aCurve, ostream &output = std::cout) {
	output << "Max X = " << aCurve.max_X << endl;
	output << "Max Y = " << aCurve.max_Y << endl;
	output << "Curve param a1 = " << aCurve.a1 << endl;
	output << "Curve param a2 = " << aCurve.a2 << endl;

	//here is for print color according to enum
	switch (aCurve.co)
	{
	case RED: output << "Color = red\n"; break;
	case ORANGE: output << "Color = orange\n"; break;
	case GREEN: output << "Color = green\n"; break;
	case CYAN: output << "Color = cyan\n"; break;
	case BLUE: output << "Color = blue\n"; break;
	}
	for (int i = 0; i < aCurve.dataX.size(); i++) {
		cout << aCurve.dataX[i]<<"    "<< aCurve.dataY[i] << endl;
	}
}



void drawCurve(SpringCurve &aCurve) {
	int key = 0;
	double x_p, y_p;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0.0, 0.0, 0.0); // set point color to black
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, aCurve.max_X + 0.1, 0, aCurve.max_Y + 1); // the viewing range is changing according to the max_X and max_Y
	glPointSize(2);
	glBegin(GL_POINTS);
	//plot all the data points 
	for (int i = 0; i < aCurve.dataX.size(); i++) {

		glVertex2f(aCurve.dataX[i], aCurve.dataY[i]);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	//switch from color to color
	switch (aCurve.co) {
		case RED: glColor3f(255, 0, 0);
			break;
		case ORANGE: glColor3ub(255, 165, 0);
			break;
		case GREEN: glColor3f(0, 255, 0);
			break;
		case CYAN: glColor3ub(0, 255, 255);
			break;
		case BLUE: glColor3ub(0, 0, 255);
			break;
		default: glColor3ub(0, 255, 0);
			break;
	}
	// here I generate thousands of points using a1 and a2, and connect them together
	for (int i = 0; i < (aCurve.max_X + 0.1) * 1000; i++) {
		x_p = i * 0.001;
		y_p = aCurve.a1 * x_p + aCurve.a2 * x_p * x_p;
		glVertex2f(x_p, y_p);
	}
	glEnd();

	glFlush();

	FsPollDevice();
	
}

int main(void) {

	int command = 0;
	int key = 0;
	char initialinput;
	SpringCurve test;
	string filename;
	//this part is to print all the instructions as a menu in consol.
	cout << "Menu" << endl;
	cout << "A->Add a curve and display it" << endl;
	cout << "C->cycle through possible colors" << endl;
	cout << "D -> delete the curve currently displayed from the collection" << endl;
	cout << "L->list all the data for the currently displayed curve on the console window" << endl;
	cout << "Arrow Left / Right->cycle through curves in the collection" << endl;
	cout << "X->eXit the program" << endl;


	//this parat is for user to enter "a" and catch "a" as a triger to let user enter the file name.
	while (command != 97) { //ascii int for "a" is 97
		command = getch();
		if (command == 97) {
			cout << "Enter file name > ";
			cin >> filename;
			createCurve(filename, test);
			FsOpenWindow(16, 16, 800, 600, 0);
			drawCurve(test);

		}

	}
	

	while (FSKEY_X != (key = FsInkey()))
	{	
		FsPollDevice();
		switch (key)
		{
		case FSKEY_C:		
			test.co = (Color)colorint;
			colorint = colorint + 1;
			if (colorint == 5) {
				colorint = 0;
			}
			drawCurve(test);
			break;
		case FSKEY_D:
			test = {};
			drawCurve(test);
			//deleteCurve(test);
			break;
		case FSKEY_A:
			cout << "Enter file name > ";
			cin >> filename;
			test = {};
			createCurve(filename, test);
			drawCurve(test);
			break;
		case FSKEY_L:
			printCurve(test);
			break;
		case FSKEY_LEFT :
			test = {};
			createCurve(inputfile[fileint], test);
			drawCurve(test);
			fileint = fileint + 1;
			if (fileint == 3) {
				fileint = 0;
			}
			break;
		}
		
		FsSleep(25);
	}


	return 0;

}

