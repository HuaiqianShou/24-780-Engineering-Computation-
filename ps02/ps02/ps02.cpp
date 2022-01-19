#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#pragma warning(disable:4996)


//Author: Huaiqian Shou 
//AndrewID: hshou
//Date:9/14/2021
//Course:24780-B
//Description of the code: Apply Least Squares Method to Quadratic Curve Fit

int main(void) {

	int i = -1;
	double x_array[500], y_array[500];
	double x = 0, y = 0,a1,a2;
	double x_sum = 0 ,y_sum = 0, x2_sum = 0,x3_sum =0 ,xy_sum = 0;
	using namespace std;
	using std::cout;
	using std::endl;
	string filename;

	cout << "Enter file name > ";
	cin >> filename;

	ifstream file(filename);
	//if (file.is_open()) {
	//	while (!file.eof()) {
	//		i = i + 1;
	//		file >> x_array[i] >> y_array[i];
	//		x_sum += 
	//		
	//	}


	//}
	if (file.is_open()) {
		while (file >> x >> y) {
			x_sum += x;
			y_sum += y;
			x2_sum += pow(x, 2);
			x3_sum += pow(x, 3);
			xy_sum += x * y;
		}
	}
	a2 = (x_sum*xy_sum -x2_sum*y_sum) / (x_sum * x3_sum - x2_sum*x2_sum);
	a1 = (-x2_sum*xy_sum+x3_sum*y_sum) / (x_sum*x3_sum-x2_sum*x2_sum);

	//cout << x_sum << endl;
	//cout << y_sum << endl;
	//cout << x2_sum << endl;
	//cout << x3_sum << endl;
	//cout << xy_sum << endl;
	//for (int j = 0; j < i; j++) {

	//	cout << x_sum <<endl;
	cout << "a1 = "<< a1 << endl;
	cout << "a2 = "<< a2 << endl;
	//}


}