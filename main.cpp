#include <iostream>
#include <fstream>
#include "stopwatch.hpp"
#include "convolution.hpp"

using namespace std;

int main(int argc, char **argv) {
	if (argc != 4) {
		cerr << argc << endl; 
		cerr << "\n" << argv[0] << " [input] [kernel] [output]"; 
		return 255;
	}

	// task 1 - get command line arguments
	const char *inputImage = argv[1];
	const char *inputMatrix = argv[2]; 
	const char *outputImage = argv[3];

	stopwatch t[3];

	// task 2 & 3 - allocate memory & read inputImage and matrix
	t[0].start(); 
	myImage data;
	if (!readImage(inputImage, data)) {
		cerr << "\nError reading the input image data, file: " << inputImage; 
		return 254;
	}
    myKernel kernel;
    if (!readKernel(inputMatrix, kernel)) {
		cerr << "\nError reading the input kernel data, file: " << inputMatrix; 
		return 254;
	}


	// task 4 - prepare resulting image matrix
	t[1].start();
	myImage result; 
    prepareResult(result, data);
	// task 5 - do the convolution Baby
	convolve(data, kernel, result); 
	t[1].stop(); 

	// task 6 - Output result image
	t[2].start();
	outputImageFile(result, outputImage); 
	t[2].stop(); 
	   	 
	// task 9 - output timing
	cout << "\n"; 
	
	cout << t[0].elapsedTime() << ", "; 
	cout << t[1].elapsedTime() << ", "; 
	cout << t[2].elapsedTime() << "\n"; 


	// task 10 - release all allocated memory
	releaseInputImage(data);
	releaseInputKernel(kernel);
	releaseOutputImage(result);

	return 0;
}