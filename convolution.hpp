#define int_p_NULL (int*)NULL
#include <string>
#include <vector>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <bits/stdc++.h>


namespace gil = boost::gil;

struct pixel {
   	int r;
   	int g;
   	int b;
};

struct myImage {
	int*** pixels;
   	int width;
   	int height;
};

struct myKernel {
	double** pixels;
	int width;
   	int height;
   	int divisor;
};

void swap(const myKernel kernel, int row1, int row2, int col);

int rankOfMatrix(const myKernel kernel);

bool readImage(std::string filename, gil::rgb8_image_t &data);

bool readKernel(std::string filename, myKernel& kernel);

void convolve(gil::rgb8_image_t &data, myKernel kernel, gil::rgb8_image_t &result);

void convolve2D(gil::rgb8_image_t &data, myKernel kernel, gil::rgb8_image_t &result);

void convolve1D(gil::rgb8_image_t &data, double kernel[],int kernelSize, int direction, gil::rgb8_image_t &result);

bool outputImageFile(const gil::rgb8_image_t &result, std::string outputImage);

void releaseInputImage(myImage &data);

void releaseInputKernel(myKernel &kernel);

void releaseOutputImage(myImage &result);
