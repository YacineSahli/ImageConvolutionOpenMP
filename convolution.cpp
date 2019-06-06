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
#include "stopwatch.hpp"

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
        // 2) If
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
bool readImage(string filename, gil::rgb8_image_t &data) {
	if(filename.substr(filename.length() - 4) == ".jpg" || filename.substr(filename.length() - 5) == ".jpeg"){
		gil::jpeg_read_image(filename, data);
	}
	else if(filename.substr(filename.length() - 4) == ".png"){
		gil::png_read_image(filename, data);
	}


	#ifdef DEBUG
		cout << "Read complete, got an image\n";
		cout << "image:" << data.width() << "x" << data.height() << "\n";
	#endif
	#ifdef STATS
		cout << data.width() << "x" << data.height() << " ";
	#endif
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
	#ifdef DEBUG
		cout << "kernel:" << kernel.width << "x" << kernel.height;
	#endif
	#ifdef STATS
		cout << kernel.width << "x" << kernel.height << " ";
	#endif
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


// Perform two 1D convolution by separating the 2D kernel
void convolve1D(gil::rgb8_image_t &data, myKernel kernel,  gil::rgb8_image_t &result){
	int kCenter0 = (kernel.width - 1) / 2;
	int kCenter1 = (kernel.height - 1) / 2;
	gil::rgb8_image_t::view_t v = view(result);
	gil::rgb8_image_t::const_view_t dataView = const_view(data);
	stopwatch t;
	double kernel1[kernel.width];
	double kernel2[kernel.height];
	for(int i = 0; i < kernel.width; i++){
		kernel1[i] = kernel.pixels[i][0];
	}
	for(int i = 0; i < kernel.height; i++){
		kernel2[i] = kernel.pixels[0][i] / kernel.pixels[0][0];
	}
	vector<double> tempResult(data.width() * data.height() * 3);
	#pragma omp parallel shared( kCenter0, kCenter1, kernel1, kernel2, dataView,data,kernel, v, result, tempResult)
	{
	#pragma omp for collapse(2) schedule(static)
	for(int i=0; i < data.width(); i++)               // rows
	{
		for(int j=0; j < data.height(); j++)          // columns
		{
			double tmpR =0;
			double tmpG =0;
			double tmpB =0;
			for(int m=0; m < kernel.width; m++) // kernel columns
			{
				// index of input signal, used for checking boundary

				int ii = i + (m - kCenter0);
				// Use the value of the closest pixel if out of bound
				if(ii < 0)
					ii = 0;
				else if(ii >= data.width())
					ii = data.width() - 1;

				gil::rgb8_pixel_t pxOriginal = dataView(0,0);
				pxOriginal = dataView(ii,j);

				tmpR += (double)pxOriginal[0] * kernel1[m];
				tmpG += (double)pxOriginal[1] * kernel1[m];
				tmpB += (double)pxOriginal[2] * kernel1[m];
			}
			if(tmpR > 255)
				tmpR = 255;
			else if(tmpR < 0)
				tmpR =0;
			if(tmpG > 255)
				tmpG = 255;
			else if(tmpG < 0)
				tmpG =0;
			if(tmpB > 255)
				tmpB = 255;
			else if(tmpB < 0)
				tmpB = 0;
			tempResult[i + data.width() * (j + data.height() * 0)] = tmpR;
			tempResult[i + data.width() * (j + data.height() * 1)] = tmpG;
			tempResult[i + data.width() * (j + data.height() * 2)] = tmpB;
		}
	}
	#pragma omp for collapse(2) schedule (static)
	for(int i=0; i < data.width(); i++)               // rows
	{
		for(int j=0; j < data.height(); j++)          // columns
		{
			double tmpR =0;
			double tmpG =0;
			double tmpB =0;
			for(int m=0; m < kernel.height; m++) // kernel columns
			{
				// index of input signal, used for checking boundary

				int ii = j + (m - kCenter1);
				// Use the value of the closest pixel if out of bound
				if(ii < 0)
					ii = 0;
				else if(ii >= data.height())
					ii = data.height() - 1;

				tmpR += tempResult[ i + data.width() * (ii + data.height() * 0)] * kernel2[m];
				tmpG += tempResult[ i + data.width() * (ii + data.height() * 1)] * kernel2[m];
				tmpB += tempResult[ i + data.width() * (ii + data.height() * 2)] * kernel2[m];
			}
			if(tmpR > 255)
				tmpR = 255;
			else if(tmpR < 0)
				tmpR =0;
			if(tmpG > 255)
				tmpG = 255;
			else if(tmpG < 0)
				tmpG =0;
			if(tmpB > 255)
				tmpB = 255;
			else if(tmpB < 0)
				tmpB = 0;
			v(i,j)[0] = tmpR;
			v(i,j)[1] = tmpG;
			v(i,j)[2] = tmpB;
		}
	}

	}
}
// This function conducts a 2D convolution.
void convolve2D(gil::rgb8_image_t &data, myKernel kernel, gil::rgb8_image_t &result){
	int kCenterX = (kernel.width - 1) / 2;
	int kCenterY = (kernel.height - 1) / 2;

	gil::rgb8_image_t::view_t v = view(result);
	gil::rgb8_image_t::const_view_t dataView = const_view(data);

	#pragma omp parallel for collapse(2) shared(dataView,kCenterX,kCenterY,data,kernel, v, result) schedule(static)
	for(int i=0; i < data.width(); i++)               // rows
	{
		for(int j=0; j < data.height(); j++)          // columns
		{
			double tmpRGB[3] ={0};
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
					else if(ii >= data.width())
						ii = data.width() - 1;
					if(jj < 0)
						jj = 0;
					else if(jj >= data.height())
						jj = data.height() - 1;

					gil::rgb8_pixel_t pxOriginal = dataView(ii,jj);
					tmpRGB[0] += pxOriginal[0] * kernel.pixels[m][n];
					tmpRGB[1] += pxOriginal[1] * kernel.pixels[m][n];
					tmpRGB[2] += pxOriginal[2] * kernel.pixels[m][n];
				}
			}
			if(tmpRGB[0] > 255)
				tmpRGB[0] = 255;
			else if(tmpRGB[0] < 0)
				tmpRGB[0] =0;
			if(tmpRGB[1] > 255)
				tmpRGB[1] = 255;
			else if(tmpRGB[1] < 0)
				tmpRGB[1] =0;
			if(tmpRGB[2] > 255)
				tmpRGB[2] = 255;
			else if(tmpRGB[2] < 0)
				tmpRGB[2] = 0;
			v(i,j)[0] = tmpRGB[0];
			v(i,j)[1] = tmpRGB[1];
			v(i,j)[2] = tmpRGB[2];
		}
	}
}
void convolve(gil::rgb8_image_t &data, myKernel kernel, gil::rgb8_image_t &result){
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
		#ifdef DEBUG
			cout << "\n\nKernel is separable, performing two 1D convolution\n\n";
		#endif

		convolve1D(data,kernel,result);
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
bool outputImageFile(const gil::rgb8_image_t &result, string outputImage) {
	fs::path outputPath(outputImage);
	if(outputPath.parent_path().string() != "" && !fs::is_directory(outputPath.parent_path())){
		fs::create_directory(outputPath.parent_path());
	}
	if(outputImage.substr(outputImage.length() - 4) == ".jpg" || outputImage.substr(outputImage.length() - 5) == ".jpeg"){
		gil::jpeg_write_view(outputImage, const_view(result));
	}
	else if(outputImage.substr(outputImage.length() - 4) == ".png"){
		gil::png_write_view(outputImage, const_view(result));
	}

	return true;
}
