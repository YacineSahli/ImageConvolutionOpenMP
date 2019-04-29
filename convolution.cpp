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
	
	data.width = img.width();
	data.height = img.height();

	data.pixels = new int**[img.width()];
	for(uint i = 0; i<img.width(); i++){
		data.pixels[i] = new int*[img.height()];
		for(uint j = 0; j<img.height(); j++){
			data.pixels[i][j] = new int[3];
			gil::rgb8_pixel_t px = *const_view(img).at(i, j);
			data.pixels[i][j][0] = (int)px[0] ;
			data.pixels[i][j][1] = (int)px[1] ;
			data.pixels[i][j][2] = (int)px[2] ;
		}
	}


	return true;
}
bool readKernel(const char *filename, myKernel& kernel) {
	fstream inp(filename);
	if (!inp) {
		cerr << "\nError opening kernel file: " << filename;
		return false;
	}
	inp >> kernel.width >> kernel.height;
	string str;
	std::getline(inp, str);  // go to next line
	kernel.pixels = new double*[kernel.width];
	kernel.divisor = 0; 
	for(int i = kernel.width - 1; i>=0; i--){ // Flip the kernel while reading
		kernel.pixels[i] = new double(kernel.height);
		for(int j = kernel.height -1; j>=0; j--){
			inp >> kernel.pixels[i][j];
			kernel.divisor += kernel.pixels[i][j];
		}
		std::getline(inp, str);
	}
	if(kernel.divisor != 0 && kernel.divisor != 1){
		for(int i = 0; i<kernel.width; i++){
			for(int j = 0; j<kernel.height; j++){
				kernel.pixels[i][j] /= kernel.divisor;
			}
		} 
	}
	return true;
}

// This function prepare the accumulator struct votes so that it will have sufficient memory for storing all votes.
void prepareResult(myImage &result, myImage data) {
	result.width = data.width;
	result.height = data.height;

	result.pixels = new int**[result.width];
	for(int i = 0; i<result.width; i++){
		result.pixels[i] = new int*[result.height];
		for(int j = 0; j<result.height; j++){
			result.pixels[i][j] = new int[3];
			result.pixels[i][j][0] = 0;
			result.pixels[i][j][1] = 0;
			result.pixels[i][j][2] = 0;
		}
	}
}



// This function conducts a 2D convolution.
void convolve2D(myImage &data, myKernel &kernel, myImage &result){
	int kCenterX = (kernel.width - 1) / 2;
	int kCenterY = (kernel.height - 1) / 2;

	for(int i=0; i < data.width; i++)               // rows
	{
		for(int j=0; j < data.height; j++)          // columns
		{
			for(int m=0; m < kernel.width; m++)     // kernel rows
			{
				for(int n=0; n < kernel.height; n++) // kernel columns
				{
					// index of input signal, used for checking boundary
					int ii = i + (m - kCenterX);
					int jj = j + (n - kCenterY);

					// Use the value of the closest pixel if out of bound
					if(ii < 0)
						ii = 0;
					else if(ii >= data.width)
						ii = data.width - 1;
					if(jj < 0)
						jj = 0;
					else if(jj >= data.height)
						jj = data.height - 1;
					
					result.pixels[i][j][0] += data.pixels[ii][jj][0] * kernel.pixels[m][n];
					result.pixels[i][j][1] += data.pixels[ii][jj][1] * kernel.pixels[m][n];
					result.pixels[i][j][2] += data.pixels[ii][jj][2] * kernel.pixels[m][n];
				}
			} 
			if(result.pixels[i][j][0] > 255){
				result.pixels[i][j][0] = 255;
			}
			else if(result.pixels[i][j][0] <0){
				result.pixels[i][j][0] = 0;
			}
			if(result.pixels[i][j][1] > 255){
				result.pixels[i][j][1] = 255;
			}
			else if(result.pixels[i][j][1] <0){
				result.pixels[i][j][1] = 0;
			}
			if(result.pixels[i][j][2] > 255){
				result.pixels[i][j][2] = 255;
			}
			else if(result.pixels[i][j][2] <0){
				result.pixels[i][j][2] = 0;
			}
		}
	}
}
void convolve(myImage &data, myKernel &kernel, myImage &result){
	convolve2D(data,kernel,result);
}


// This function release the allocated memory for the input image.
void releaseInputImage(myImage &data) {
	for(int i = 0; i<data.width; i++){
		for(int j = 0; j<data.height; j++){
			delete[] data.pixels[i][j];
		}
		delete[] data.pixels[i];
	}
	delete[] data.pixels;
}

// This function de-allocate memory allocated for kernel
void releaseInputKernel(myKernel &kernel) {
	for(int i = 0; i<kernel.width; i++){
		delete[] kernel.pixels[i];
	}
	delete[] kernel.pixels;
}

// task 10 - release allocated memory for output image
void releaseOutputImage(myImage &result) {
	for(int i = 0; i<result.width; i++){
		for(int j = 0; j<result.height; j++){
			delete[] result.pixels[i][j];
		}
		delete[] result.pixels[i];
	}
	delete[] result.pixels;
}

// Task 8a - Output transformed point cloud data
bool outputImageFile(const myImage &result, const char *outputImage) {
	
	gil::rgb8_image_t toWrite(result.width, result.height);
	gil::rgb8_image_t::view_t v = view(toWrite);
	for(int i=0; i< result.width; i++){
		for(int j=0; j< result.height;j++){
			v(i, j) = gil::rgb8_pixel_t(result.pixels[i][j][0], result.pixels[i][j][1], result.pixels[i][j][2]); 
		}
	}
	gil::jpeg_write_view(outputImage, const_view(toWrite)); 

	return true;
}