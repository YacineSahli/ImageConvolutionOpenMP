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


// Perform a 1D convolution, direction : 0 horizontal, 1 vertical
void convolve1D(gil::rgb8_image_t &data, double kernel[],int kernelSize, int direction, gil::rgb8_image_t &result){
	int kCenter = (kernelSize - 1) / 2;
	gil::rgb8_image_t::view_t v = view(result);
	gil::rgb8_image_t::const_view_t dataView = const_view(data);
	#pragma omp parallel for collapse(2) default(none) shared(result) firstprivate(dataView,v,direction,kCenter,data,kernelSize,kernel)
	for(int i=0; i < data.width(); i++)               // rows
	{
		for(int j=0; j < data.height(); j++)          // columns
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
				else if(direction == 0 && ii >= data.width())
					ii = data.width() - 1;
				else if(direction == 1 && ii >= data.height())
					ii = data.height() - 1;
				if(direction == 0){
					gil::rgb8_pixel_t pxOriginal = dataView(ii,j);
					gil::rgb8_pixel_t px = *const_view(result).at(i,j);
					v(i,j) = gil::rgb8_pixel_t((int)px[0] + (int)pxOriginal[0] * kernel[m],
								   (int)px[1] + (int)pxOriginal[1] * kernel[m],
								   (int)px[2] + (int)pxOriginal[2] * kernel[m]);
				}
				else if(direction == 1){
					gil::rgb8_pixel_t pxOriginal = dataView(i,ii);
					gil::rgb8_pixel_t px = *const_view(result).at(i,j);
					v(i,j) = gil::rgb8_pixel_t((int)px[0] + (int)pxOriginal[0] * kernel[m],
								   (int)px[1] + (int)pxOriginal[1] * kernel[m],
								   (int)px[2] + (int)pxOriginal[2] * kernel[m]);
				}
			}
			/*if(result.pixels[i][j][0] > 255)
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
				result.pixels[i][j][2] = 0;*/
		}
	}
}
// This function conducts a 2D convolution.
void convolve2D(gil::rgb8_image_t &data, myKernel kernel, gil::rgb8_image_t &result){
	int kCenterX = (kernel.width - 1) / 2;
	int kCenterY = (kernel.height - 1) / 2;

	gil::rgb8_image_t::view_t v = view(result);
	gil::rgb8_image_t::const_view_t dataView = const_view(data);

	#pragma omp parallel for collapse(2) default(none) shared(result) firstprivate(dataView,v,kCenterX,kCenterY,data,kernel)
	for(int i=0; i < data.width(); i++)               // rows
	{
		for(int j=0; j < data.height(); j++)          // columns
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
					else if(ii >= data.width())
						ii = data.width() - 1;
					if(jj < 0)
						jj = 0;
					else if(jj >= data.height())
						jj = data.height() - 1;

					gil::rgb8_pixel_t pxOriginal = dataView(ii,jj);
					gil::rgb8_pixel_t px = *const_view(result).at(i,j);
					v(i,j) = gil::rgb8_pixel_t((int)px[0] + (int)pxOriginal[0] * kernel.pixels[m][n],
								   (int)px[1] + (int)pxOriginal[1] * kernel.pixels[m][n],
								   (int)px[2] + (int)pxOriginal[2] * kernel.pixels[m][n]);
				}
			}
			/*if(result.pixels[i][j][0] > 255)
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
				result.pixels[i][j][2] = 0;*/
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
	if(kernel.width >4 && kernel.height >4 && rankOfMatrix(kernelCopy) == 1){
		#ifdef DEBUG
			cout << "\n\nKernel is separable, performing two 1D convolution\n\n";
		#endif
		double kernel1[kernel.width];
		double kernel2[kernel.height];
		for(int i = 0; i < kernel.width; i++){
			kernel1[i] = kernel.pixels[i][0];
		}
		for(int i = 0; i < kernel.height; i++){
			kernel2[i] = kernel.pixels[0][i] / kernel.pixels[0][0];
		}
		gil::rgb8_image_t resultTemp(data.width(), data.height());

		convolve1D(data,kernel1,kernel.width,0,resultTemp);
		convolve1D(resultTemp,kernel2,kernel.height,1,result);
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
