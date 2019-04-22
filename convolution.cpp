#include "convolution.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <boost/gil/extension/io/jpeg_io.hpp>

using namespace std;
namespace gil = boost::gil;

// Read the original point cloud data
bool readImage(const char *filename, myImage& data) {
	gil::rgb8_image_t img;
	gil::jpeg_read_image(filename, img);
	std::cout << "Read complete, got an image " << img.width()
			<< " by " << img.height() << " pixels\n";
	gil::rgb8_pixel_t px = *const_view(img).at(5, 10);
	std::cout << "The pixel at 5,10 is "
				<< (int)px[0] << ','
				<< (int)px[1] << ','
				<< (int)px[2] << '\n';
	// you should now allocate enough memory to store all the data...



	return true;
}
bool readKernel(const char *filename, myKernel& kernel) {
	

	return true;
}

// This function prepare the accumulator struct votes so that it will have sufficient memory for storing all votes.
void prepareResult(myImage &result) {

}



// This function conducts the Hough Transform to cast votes in the rho, theta, phi parametric space.
void convolve(myImage &data, myKernel &kernel, myImage &result){
	
}


// This function release the allocated memory for the input image.
void releaseInputImage(myImage &data) {
}

// This function de-allocate memory allocated for kernel
void releaseInputKernel(myKernel &kernel) {
}

// task 10 - release allocated memory for output image
void releaseOutputImage(myImage &result) {
}

// Task 8a - Output transformed point cloud data
bool outputImageFile(const myImage &result, const char *outputImage) {
	ofstream outp(outputImage);
	if (!outp) return false; 


	outp.close(); 
	return true;
}