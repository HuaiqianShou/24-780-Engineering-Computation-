/*
Nestor Gomez & Huaiqian Shou
Carnegie Mellon University
24-780-B Engineering Computation
PS 09: Matrices

Due: Tues, Nov. 23, 2021
Description: Test cases for matrix functions
*/

#include <iostream>
#include <chrono>
#include "ps09_matrix2D_hshou.h"


int main() {
	using namespace std;

	cout << " MATRIX TESTER " << endl;

	//Matrix2D<float, 4, 4> mat4, mat4a;

	Matrix2D<float, 4, 4> mat4;
	mat4.readFile("matrix4x4.txt");

	cout << "Matrix 4x4 read:" << endl;
	mat4.print();


	//Matrix2D<float, 4, 4> mat4Other(mat4); // using copy constructor
	auto begin = std::chrono::high_resolution_clock::now();

	mat4.transpose();

	double timeElapsed = chrono::duration_cast<chrono::nanoseconds>
		(std::chrono::high_resolution_clock::now() - begin).count() / 1e9;

	cout << "Matrix 4x4 transposed:" << endl;
	mat4.print();
	cout << "Transpose of 4x4: elapsed " << timeElapsed << " sec." << endl;

	// deep copy test
	Matrix2D<float, 4, 4> mat4Transp;
	mat4Transp = mat4; // copy the transposed matrix
	mat4.transpose();  // set it back to how it was

	cout << "COPY of Matrix 4x4 transposed:" << endl;
	mat4Transp.print();

	//multiplying
	Matrix2D<float, 4, 4> result4;
	begin = std::chrono::high_resolution_clock::now();

	//mat4.matrixMultiplyParallel(mat4, result4);

	timeElapsed = chrono::duration_cast<chrono::nanoseconds>
		(std::chrono::high_resolution_clock::now() - begin).count() / 1e9;

	cout << "Matrix multiply result (squaring):" << endl;
	result4.print();
	cout << "Squaring of 4x4: elapsed " << timeElapsed << " sec." << endl;


	// ==================================
	Matrix2D<double, 500, 500> mat500;
	mat500.readFile("matrix500x500.txt");

	// multiplying
	Matrix2D<double, 500, 500> result500;
	begin = std::chrono::high_resolution_clock::now();

	mat500.matrixMultiplyParallel(mat500, result500);

	timeElapsed = chrono::duration_cast<chrono::nanoseconds>
		(std::chrono::high_resolution_clock::now() - begin).count() / 1e9;

	cout << "Matrix multiply result (squaring):" << endl;
	result500.print(cout, 10);
	cout << "Squaring of 500x500: elapsed " << timeElapsed << " sec." << endl;



	Matrix2D<double, 5000, 5000> mat5000;
	mat5000.readFile("matrix5000x5000.txt");

	// multiplying
	Matrix2D<double, 5000, 5000> result5000;

	begin = std::chrono::high_resolution_clock::now();

	mat5000.matrixMultiplyParallel(mat5000, result5000);

	timeElapsed = chrono::duration_cast<chrono::nanoseconds>
		(std::chrono::high_resolution_clock::now() - begin).count() / 1e9;

	cout << "Matrix multiply result (squaring):" << endl;
	result5000.print(cout, 10);

	cout << "Squaring of 5000x5000: elapsed " << timeElapsed << " sec." << endl;


}