#pragma once
#include <iostream>
#include<fstream>
#include<string>
#include<iterator>
#include <iomanip>  
#include<vector>
#include<algorithm>
#include<thread>


/*
Huaiqian Shou
Carnegie Mellon University
24-780-B Engineering Computation
PS 09: Matrices

Due: Tues, Nov. 23, 2021
Description: Follow Prof through the lecture to complete the basic function of matrix 
and finish matrix transpose and multiplication
*/



template<class T, const int NR, const int NC>

class Matrix2D {

private:
	T* content;
public:
	// Class constructor and destructor 
	Matrix2D() { content = new T[NR * NC]; };

	~Matrix2D() { delete[] content; };
	
	
	
	// Sets value at given location, rows and columns are numbered 1 to whatever 
	// Use the following if row or col are out-of-range 
	// ->> throw std::out_of_range("Bad matrix operation"); 
	void set(int row, int col, const T& incoming);
	
	
	// Gets value at given location, throwing error similar to set() 
	const T& value(int row, int col) const;


	// Loads matrix data from the file given 
	void readFile(const std::string& fileName);
	
	
	
	
	
	
	// Prints the whole array to output, defaulting to cout. 
	// If positive limit is given, only the first printLimit rows and columns 
	// are printed rather than whole matrix (useful for checking big matrices). 
	void print(std::ostream& output = std::cout, int printLimit = -1) const;
	
	
	
	
	
	
	
	// copy constructor 
	Matrix2D(const Matrix2D<T, NR, NC>& original);
	
	
	
	// assignment operator 
	Matrix2D<T, NR, NC>& operator=(const Matrix2D<T, NR, NC>& original);
	
	
	
	// Transposes the matrix in place. 
	void transpose();
	
	
	// multiply the matrix by a scalar 
	void scalarMultiply(const T& factor);

	// Multiplies the matrix with otherMatrix and stores the result in the 
	// resultMatrix. Note that otherMatrix is the right-hand operand (since matrix 
	// multiplication is NOT commutative) 
	/*void matrixMultiply(Matrix2D<T, NC, NR>& otherMatrix, Matrix2D <T, NR, NC>& resultMatrix);*/



	// Multiplies the matrix with otherMatrix and stores the result in the 
	// resultMatrix. Note that otherMatrix is the right-hand operand (since matrix 
	// multiplication is NOT commutative). Use optional parameters for portioning. 
	void matrixMultiply(Matrix2D<T, NC, NR>& otherMatrix,Matrix2D <T, NR, NC>& resultMatrix,int startRow = 1, int endRow = NR, int startCol = 1, int endCol = NC);


	// similar to matrixMultiply, but uses multi-threading 
	void matrixMultiplyParallel(Matrix2D<T, NC, NR>& otherMatrix,Matrix2D <T, NR, NR>& resultMatrix);

public: static void threadEntry(Matrix2D<T, NR, NC>* thisPtr);

	  Matrix2D<T, NR, NC>* rightHandMatrixPtr;
	  Matrix2D<T, NR, NC>* resultMatrixPtr;

	  int currThread;


};

//template <class T, const int NR, const int NC>
//class MatrixMultParams {
//
//};


template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::set(int row, int col, const T& incoming)
{
	// matrices go from 1 to whatever
	// arrays go from zero to whatever

	if (1 <= row && row <= NR && 1 <= col && col <= NC) {
		content[(row - 1) * NC + col - 1] = incoming;
	}
	else {
		throw std::out_of_range("Bad matrix operation on write.");
	}
}

template<class T, int NR, int NC>
inline const T& Matrix2D<T, NR, NC>::value(int row, int col) const
{
	// TODO: insert return statement here
	if (1 <= row && row <= NR && 1 <= col && col <= NC) {
		return content[(row - 1) * NC + col - 1];
	}
	else {
		throw std::out_of_range("Bad matrix operation on read.");
	}
}

template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::readFile(const std::string& fileName)
{

	using namespace std;

	ifstream inFile(fileName);

	if (inFile.is_open()) {
		for (int i = 0; i < NR; i++) {
			for (int j = 0; j < NC; j++) {
				inFile >> content[i * NC + j];
			}
		}
		
		
	}
	inFile.close();
}

template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::print(std::ostream& output, int printLimit) const
{
	using namespace std;

	int rowLimit = NR;
	int colLimit = NC;

	if (printLimit > 0) {

		rowLimit = min(NR, printLimit);
		colLimit = min(NC, printLimit);

	}


	for (int i = 0; i < rowLimit; i++) {
		for (int j = 0; j < colLimit; j++) {
			output << setw(10) << setprecision(3) << content[i * NC + j] << " ";
		}
		output << endl;
	}

}

template<class T, int NR, int NC>
inline Matrix2D<T, NR, NC>::Matrix2D(const Matrix2D<T, NR, NC>& original)
{

	//copy all the mumber variables


	//create a new dynamic 
	content = new T[NR * NC];

	//copy into dynamic stuff using loop
	for (int i = 0; i < NR * NC; i++) {
		content[i] = original.content[i];
	}

	//for (int i = 1; i <= NR; i++) {
	//	for (int j = 1; j < +NC; j++) {
	//		set(i, j, original.get(i, j));
	//	}
	//}

}

template<class T, int NR, int NC>
inline Matrix2D<T, NR, NC>& Matrix2D<T, NR, NC>::operator=(const Matrix2D<T, NR, NC>& original)
{
	// TODO: insert return statement here
	for (int i = 0; i < NR * NC; i++) {
		content[i] = original.content[i];
	}

	return *this;
}

template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::transpose()
{

	for (int i = 0; i < NR; i++) {
		for (int j = 0; j < i; j++) {
			T a;
			T b;
			a = content[i + j * NC];
			b = content[i * NC + j];
			content[i * NC + j] = a;
			content[i + j * NC] = b;
		}
	}
}

template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::scalarMultiply(const T& factor)
{
	for (int i = 0; i < NR * NC; i++) {
		content[i] = content[i] * factor;
	}

}

//template<class T, int NR, int NC>
//inline void Matrix2D<T, NR, NC>::matrixMultiply(Matrix2D<T, NC, NR>& otherMatrix, Matrix2D<T, NR, NC>& resultMatrix)
//{
//	for (int i = 0; i < NR; i++) {
//		for (int j = 0; j < NC; j++) {
//			
//			
//			
//			resultMatrix.content[i * NC + j] = 0.0;
//			
//		}
//	}
//	
//	
//	for (int i = 0; i < NR; i++) {
//		for (int j = 0; j < NC; j++) {
//			for (int k = 0; k < NC; k++) {
//				resultMatrix.content[i * NC + j] += content[i * NC + k] * otherMatrix.content[k * NC + j];
//			}
//		}
//	}
//
//}

template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::matrixMultiply(Matrix2D<T, NC, NR>& otherMatrix, Matrix2D<T, NR, NC>& resultMatrix, int startRow, int endRow, int startCol, int endCol)
{


	//for (int i = 0; i < NR; i++) {
	//	for (int j = 0; j < NC; j++) {



	//		resultMatrix.content[i * NC + j] = 0.0;

	//	}
	//}


	//for (int i = startRow-1; i < endRow; i++) {
	//	for (int j = startCol-1; j < endCol; j++) {
	//		for (int k = startCol - 1; k < endCol; k++) {
	//			resultMatrix.content[i * NC + j] += content[i * NC + k] * otherMatrix.content[k * NC + j];
	//		}
	//	}
	//}


	T tempSum;
	for (int i = startRow; i <= endRow; i++) {
		for (int j = startCol; j <= endCol; j++) {
			tempSum = 0;
			for (int k = startCol; k <= endCol; k++)
				tempSum += value(i, k) * otherMatrix.value(k, j);

			resultMatrix.set(i, j, tempSum);
		}
	}

}

template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::matrixMultiplyParallel(Matrix2D<T, NC, NR>& otherMatrix, Matrix2D<T, NR, NR>& resultMatrix)
{
	
	int multiThreadCutoff = 10;


	if (NR <= multiThreadCutoff) {

		matrixMultiply(otherMatrix, resultMatrix);
	}		
	else {
		using namespace std;
		int numbThreads = 4;
		//prepare for multi-threading
		rightHandMatrixPtr = &otherMatrix;
		resultMatrixPtr = &resultMatrix;
		this->currThread = 1;
		int lastThread;

		vector<thread> theThreads;
		theThreads.resize(numbThreads);

		//start the threads in a loop
		for (int i = 0; i < numbThreads; i++) {
			// start a thread
			lastThread = this->currThread;
			thread aThread(Matrix2D<T, NR, NC>::threadEntry, this);
			// start thread into vector
			theThreads[i].swap(aThread);
			//currThread++;
			while (lastThread == this->currThread);
		}

		// wait for all threads to finish (join the threads)
		for (int i = 0; i < numbThreads; i++) {
			theThreads[i].join();
		}

	}
}

template<class T, int NR, int NC>
inline void Matrix2D<T, NR, NC>::threadEntry(Matrix2D<T, NR, NC>* thisPtr)
{

	switch (thisPtr->currThread) {
	case 1:  
		thisPtr->currThread++;
		thisPtr->matrixMultiply(*(thisPtr->rightHandMatrixPtr), *(thisPtr->resultMatrixPtr), 1, NR / 2, 1, NC / 2);
		break;
	case 2:
		thisPtr->currThread++;
		thisPtr->matrixMultiply(*(thisPtr->rightHandMatrixPtr), *(thisPtr->resultMatrixPtr), 1, NR / 2, NC / 2 + 1, NC);
		break;
	case 3:
		thisPtr->currThread++;
		thisPtr->matrixMultiply(*(thisPtr->rightHandMatrixPtr), *(thisPtr->resultMatrixPtr), NR / 2 + 1, NR, 1, NC / 2);
		break;
	case 4:
		thisPtr->currThread++;
		thisPtr->matrixMultiply(*(thisPtr->rightHandMatrixPtr), *(thisPtr->resultMatrixPtr), NR / 2 + 1, NR,  NC / 2 + 1, NC);
		break;
	}
}
