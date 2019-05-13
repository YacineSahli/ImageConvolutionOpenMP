#include <string>
#include <vector>

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

void prepareResult(myImage &result, myImage data);

bool readImage(std::string filename, myImage& data);

bool readKernel(std::string filename, myKernel& kernel);

void convolve(myImage &data, myKernel &kernel, myImage &result);

void convolve2D(myImage &data, myKernel &kernel, myImage &result);

void convolve1D(myImage &data, myKernel &kernel, myImage &result);

bool outputImageFile(const myImage &result, std::string outputImage);

void releaseInputImage(myImage &data);

void releaseInputKernel(myKernel &kernel);

void releaseOutputImage(myImage &result);
