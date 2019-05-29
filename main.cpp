#include <iostream>
#include <fstream>
#include "stopwatch.hpp"
#include "convolution.hpp"
#include <string>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;
namespace fs = boost::filesystem;
void processImage(string inputImage, string inputMatrix, string outputImage){

	stopwatch t[3];
	if(inputImage.substr(inputImage.length() - 4) != ".png" && inputImage.substr(inputImage.length() - 4) != ".jpg" && inputImage.substr(inputImage.length() - 5) != ".jpeg" && inputImage.substr(inputImage.length() - 4) != ".bmp"){
		cerr << "\n"  << " [input](.png;.jpg;.jpeg;.bmp) [kernel] [output](.png;.bmp)\n";
	}

	if(outputImage.substr(outputImage.length() - 4) != ".png" && outputImage.substr(outputImage.length() - 4) != ".jpg" && outputImage.substr(outputImage.length() - 5) != ".jpeg" && outputImage.substr(outputImage.length() - 4) != ".bmp"){
		cerr << "\n" << " [input](.png;.jpg;.jpeg;.bmp) [kernel] [output](.png;.bmp)\n";
	}
	// task 2 & 3 - allocate memory & read inputImage and matrix
	t[0].start();
	myImage data;
	if (!readImage(inputImage, data)) {
		cerr << "\nError reading the input image data, file: " << inputImage;
	}
    	myKernel kernel;
    	if (!readKernel(inputMatrix, kernel)) {
		cerr << "\nError reading the input kernel data, file: " << inputMatrix;
	}
	t[0].stop();


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


	//cout << t[0].elapsedTime() << ", ";
	//cout << t[1].elapsedTime() << ", ";
	//cout << t[2].elapsedTime() << "\n";
	#ifdef DEBUG
		cout << "\n";
		cout << "convolution time:" << t[1].elapsedTime() << "\n";
		cout << "total time:" << t[0].elapsedTime() + t[1].elapsedTime() + t[2].elapsedTime() << "\n\n";
	#endif

	#ifdef STATS
		cout << omp_get_max_threads() << " ";
		cout << t[0].elapsedTime() << " " << t[1].elapsedTime() << " " << t[2].elapsedTime() << "\n";
	#endif
	// task 10 - release all allocatedomp environment variable memory
	releaseInputImage(data);
	releaseInputKernel(kernel);
	releaseOutputImage(result);
}
int main(int argc, char **argv) {
	if (argc != 4) {
		cerr << argc << endl;
		cerr << "\n" << argv[0] << " [input] [kernel] [output]\n";
		return 255;
	}
	// task 1 - get command line arguments
	string inputImage = argv[1];
	string inputMatrix = argv[2];
	string outputImage = argv[3];



	if(fs::is_directory(inputImage)){
		std::vector<fs::directory_entry> v;
		copy(fs::directory_iterator(inputImage), fs::directory_iterator(), back_inserter(v));
		for(std::vector<fs::directory_entry>::const_iterator entry = v.begin(); entry < v.end(); ++entry){
			fs::path dstFolder = outputImage;
			if(!fs::exists(dstFolder) || !fs::is_directory(dstFolder)){
     				fs::create_directory(dstFolder);
			}
            		processImage((*entry).path().string(), inputMatrix, outputImage + "/convolved_" + (*entry).path().filename().string());
		}
	}
	else if (fs::is_regular_file(inputImage)){
        	processImage(inputImage, inputMatrix, outputImage);
	}

	return 0;
}
