/*
Nestor Gomez & Huaiqian Shou
Carnegie Mellon University
Eng. Computation, 24-780-B
Prob Set 7
Due Fri. Oct. 29, 2021

Follow Professor's instruction finish the coding of Lshape and Ishape class to deal with different shapes.
*/

#include <iostream>

#include "fssimplewindow.h"

#include "ViewManager.h"


using namespace std;

int main(void)
{
	// set up graphics window
	FsOpenWindow(16, 16, WIN_WIDTH, WIN_HEIGHT, 1, "Shape Controller (NG 2021)");

	ViewManager theManager;

	// show menu on console
	theManager.showMenu();

	// run user input loop
	while (theManager.manage()) {
		FsSleep(25);
	}

	// goodbye message and wait for user confirmation
	cout << endl << "Thank you for using our shape viewing program." << endl;
	cout << "\t\t Press ENTER to close console" << endl;

	while (getchar() != '\n'); // this will clear the newline associated with last input
	getchar();  // this will wait for enter
}


