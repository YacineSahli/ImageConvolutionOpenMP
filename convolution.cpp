#define int_p_NULL (int*)NULL

#include "convolution.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include "boost/filesystem.hpp"
#include <bits/stdc++.h>

using namespace std;
namespace gil = boost::gil;
namespace fs = boost::filesystem;

void swap(const myKernel kernel, int row1, int row2, int col){
    for (int i = 0; i < col; i++){
        int temp = kernel.pixels[row1][i];
        kernel.pixels[row1][i] = kernel.pixels[row2][i];
        kernel.pixels[row2][i] = temp;
    }
}


/* function for finding rank of matrix  https://www.geeksforgeeks.org/program-for-rank-of-matrix*/
int rankOfMatrix(const myKernel kernel){
    int rank = kernel.height;

    for (int row = 0; row < rank; row++){
        // Before we visit current row 'row', we make
        // sure that mat[row][0],....mat[row][row-1]
        // are 0.

        // Diagonal element is not zero
        if (kernel.pixels[row][row]){
           for (int col = 0; col < kernel.width; col++){
               if (col != row){
                 // This makes all entries of current
                 // column as 0 except entry 'mat[row][row]'
                 double mult = (double)kernel.pixels[col][row] /
                                       kernel.pixels[row][row];
                 for (int i = 0; i < rank; i++)
                   kernel.pixels[col][i] -= mult * kernel.pixels[row][i];
              }
           }
        }

        // Diagonal element is already zero. Two cases
        // arise:
        // 1) If there is a row below it with non-zero
        //    entry, then swap this row with that row
        //    and process that row
        // 2) If all elements in current column below
        //    mat[r][row] are 0, then remvoe this column
        //    by swapping it with last column and
        //    reducing number of columns by 1.
        else
        {
            bool reduce = true;

            /* Find the non-zero element in current
                column  */
            for (int i = row + 1; i < kernel.width;  i++)
            {
                // Swap the row with non-zero element
                // with this row.
                if (kernel.pixels[i][row])
                {
                    swap(kernel, row, i, rank);
                    reduce = false;
                    break ;
                }
            }

            // If we did not find any row with non-zero
            // element in current columnm, then all
            // values in this column are 0.
            if (reduce)
            {
                // Reduce number of columns
                rank--;

                // Copy the last column here
                for (int i = 0; i < kernel.width; i ++)
                    kernel.pixels[i][row] = kernel.pixels[i][rank];
            }

            // Process this row again
            row--;
        }

       // Uncomment these lines to see intermediate results
       // display(mat, R, C);
       // printf("\n");
    }
    return rank;
}

// Read the original point cloud data
bool readImage(string filename, myImage& data) {
	gil::rgb8_image_t img;

	if(filename.substr(filename.length() - 4) == ".jpg" || filename.substr(filename.length() - 5) == ".jpeg"){
		gil::jpeg_read_image(filename, img);
	}
	else if(filename.substr(filename.length() - 4) == ".png"){
		gil::png_read_image(filename, img);
	}

	cout << "Read complete, got an image\n";
	cout << "image:" << img.width() << "x" << img.height() << "\n";

	data.width = img.width();
	data.height = img.height();

	data.pixels = new int**[data.width];

	gil::rgb8_pixel_t px = *const_view(img).at(0, 0);

	for(int i = 0; i<data.width; i++){
		data.pixels[i] = new int*[data.height];
		for(int j = 0; j<data.height; j++){
			data.pixels[i][j] = new int[3];
			px = *const_view(img).at(i, j);
			data.pixels[i][j][0] = (int)px[0] ;
			data.pixels[i][j][1] = (int)px[1] ;
			data.pixels[i][j][2] = (int)px[2] ;
		}
	}
	return true;
}
bool readKernel(string filename, myKernel &kernel) {
	fstream inp(filename);
	if (!inp) {
		cerr << "\nError opening kernel file: " << filename;
		return false;
	}
	inp >> kernel.width >> kernel.height;
	string str;
	cout << "kernel:" << kernel.width << "x" << kernel.height;
	std::getline(inp, str);  // go to next line
	kernel.pixels = new double*[kernel.width];
	kernel.divisor = 0.0;
	for(int i = kernel.width - 1; i>=0; i--){ // Flip the kernel while reading
		kernel.pixels[i] = new double[kernel.height];
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


// Perform a 1D convolution, direction : 0 horizontal, 1 vertical
void convolve1D(myImage &data, double kernel[],int kernelSize, int direction, myImage &result){
	int kCenter = (kernelSize - 1) / 2;

	for(int i=0; i < data.width; i++)               // rows
	{
		for(int j=0; j < data.height; j++)          // columns
		{
			for(int m=0; m < kernelSize; m++) // kernel columns
			{
				// index of input signal, used for checking boundary

				int ii;
				if(direction == 0)
					ii = i + (m - kCenter);
				else if(direction == 1)
					ii = j + (m - kCenter);
				// Use the value of the closest pixel if out of bound
				if(ii < 0)
					ii = 0;
				else if(direction == 0 && ii >= data.width)
					ii = data.width - 1;
				else if(direction == 1 && ii >= data.height)
					ii = data.height - 1;
				if(direction == 0){
					result.pixels[i][j][0] += data.pixels[ii][j][0] * kernel[m];
					result.pixels[i][j][1] += data.pixels[ii][j][1] * kernel[m];
					result.pixels[i][j][2] += data.pixels[ii][j][2] * kernel[m];
				}
				else if(direction == 1){
					result.pixels[i][j][0] += data.pixels[i][ii][0] * kernel[m];
					result.pixels[i][j][1] += data.pixels[i][ii][1] * kernel[m];
					result.pixels[i][j][2] += data.pixels[i][ii][2] * kernel[m];
				}
			}

			if(result.pixels[i][j][0] > 255)
				result.pixels[i][j][0] = 255;
			else if(result.pixels[i][j][0] < 0)
				result.pixels[i][j][0] = 0;
			if(result.pixels[i][j][1] > 255)
				result.pixels[i][j][1] = 255;
			else if(result.pixels[i][j][1] < 0)
				result.pixels[i][j][1] = 0;
			if(result.pixels[i][j][2] > 255)
				result.pixels[i][j][2] = 255;
			else if(result.pixels[i][j][2] < 0)
				result.pixels[i][j][2] = 0;
		}
	}
}
// This function conducts a 2D convolution.
void convolve2D(myImage &data, myKernel kernel, myImage &result){

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
			if(result.pixels[i][j][0] > 255)
				result.pixels[i][j][0] = 255;
			else if(result.pixels[i][j][0] < 0)
				result.pixels[i][j][0] = 0;
			if(result.pixels[i][j][1] > 255)
				result.pixels[i][j][1] = 255;
			else if(result.pixels[i][j][1] < 0)
				result.pixels[i][j][1] = 0;
			if(result.pixels[i][j][2] > 255)
				result.pixels[i][j][2] = 255;
			else if(result.pixels[i][j][2] < 0)
				result.pixels[i][j][2] = 0;
		}
	}
}
void convolve(myImage &data, myKernel kernel, myImage &result){
	myKernel kernelCopy;
	kernelCopy.width = kernel.width;
	kernelCopy.height = kernel.height;
	kernelCopy.divisor = kernel.divisor;
	kernelCopy.pixels = new double*[kernelCopy.width];
	for(int i = 0; i < kernel.width; i++){
		kernelCopy.pixels[i] = new double[kernel.height];
		for(int j = 0; j< kernel.height; j++){
			kernelCopy.pixels[i][j] = kernel.pixels[i][j];
		}
	}
	if(rankOfMatrix(kernelCopy) == 1){
		cout << "\n\nKernel is separable, performing two 1D convolution\n\n";
		double kernel1[kernel.width];
		double kernel2[kernel.height];
		for(int i = 0; i < kernel.width; i++){
			kernel1[i] = kernel.pixels[i][0];
		}
		for(int i = 0; i < kernel.height; i++){
			kernel2[i] = kernel.pixels[0][i] / kernel.pixels[0][0];
		}
		myImage resultTemp;
		resultTemp.width = data.width;
		resultTemp.height = data.height;

		resultTemp.pixels = new int**[resultTemp.width];
		for(int i = 0; i<resultTemp.width; i++){
			resultTemp.pixels[i] = new int*[resultTemp.height];
			for(int j = 0; j<resultTemp.height; j++){
				resultTemp.pixels[i][j] = new int[3];
				resultTemp.pixels[i][j][0] = 0;
				resultTemp.pixels[i][j][1] = 0;
				resultTemp.pixels[i][j][2] = 0;
			}

		}
		convolve1D(data,kernel1,kernel.width,0,resultTemp);
		convolve1D(resultTemp,kernel2,kernel.height,1,result);
		releaseOutputImage(resultTemp);
	}
	else{
		convolve2D(data,kernel,result);
	}
	releaseInputKernel(kernelCopy);
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

// release allocated memory for output image
void releaseOutputImage(myImage &result) {
	for(int i = 0; i<result.width; i++){
		for(int j = 0; j<result.height; j++){
			delete[] result.pixels[i][j];
		}
		delete[] result.pixels[i];
	}
	delete[] result.pixels;
}

// Output transformed point cloud data
bool outputImageFile(const myImage &result, string outputImage) {

	gil::rgb8_image_t toWrite(result.width, result.height);
	gil::rgb8_image_t::view_t v = view(toWrite);
	for(int i=0; i< result.width; i++){
		for(int j=0; j< result.height;j++){
			v(i, j) = gil::rgb8_pixel_t(result.pixels[i][j][0], result.pixels[i][j][1], result.pixels[i][j][2]);
		}
	}
	fs::path outputPath(outputImage);
	if(outputPath.parent_path().string() != "" && !fs::is_directory(outputPath.parent_path())){
		fs::create_directory(outputPath.parent_path());
	}
	if(outputImage.substr(outputImage.length() - 4) == ".jpg" || outputImage.substr(outputImage.length() - 5) == ".jpeg"){
		gil::jpeg_write_view(outputImage, const_view(toWrite));
	}
	else if(outputImage.substr(outputImage.length() - 4) == ".png"){
		gil::png_write_view(outputImage, const_view(toWrite));
	}

	return true;
}
