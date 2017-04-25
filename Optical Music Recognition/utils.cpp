#include <algorithm>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include<cmath>

#include "utils.h"
#include "Kernel.h"
#include "Canny.h"
#include "SImageIO.h"
#include "TemplateDetector.h"
#include "A1Debug.h"
#include "StaffDetector.h"


using namespace std;

/*std::vector<DetectedSymbol> score_using_edgemaps(const SDoublePlane& input) //question 5
{

	//generating binary edge maps using canny
	//SDoublePlane image_binary_edgemap = convert_to_binary_edgemap(canny(input, 400, 0));
	SDoublePlane temp = SImageIO::read_png_file("template2.png");
	//SDoublePlane template_binary_edgemap = convert_to_binary_edgemap(canny(temp, 400, 0));
	SDoublePlane template_binary_edgemap = convert_to_binary_edgemap(get_sobel(temp));

	//edgemaps using soble
	SDoublePlane image_binary_edgemap = convert_to_binary_edgemap(get_sobel(input));
	(void)get_sobel(temp);
	SDoublePlane temp_image_binary_edgemap = threshold(image_binary_edgemap, 0.5, THRESH_ZERO, THRESH_MAX);
	SDoublePlane temp_template_binary_edgemap = threshold(template_binary_edgemap, 0.5, THRESH_ZERO, THRESH_MAX);
	SImageIO::write_png_file("image_binary_edgemap.png", temp_image_binary_edgemap, temp_image_binary_edgemap, temp_image_binary_edgemap);
	SImageIO::write_png_file("template_binary_edgemap.png", temp_template_binary_edgemap, temp_template_binary_edgemap, temp_template_binary_edgemap);
	//std::vector<DetectedSymbol> symbols1;
	//return symbols1;

	//SDoublePlane dist_score = SImageIO::read_png_file("1234.png");

	//calculating the the distance matrix D
	cout<<"starting distance calculator.."<<endl;
	SDoublePlane D_mat = dist(image_binary_edgemap);
	
	//reading directly from the dist_matrix image saved
	//SDoublePlane D_mat = SImageIO::read_png_file("music1_dist_matrix.png");

	//cout<<"writing the distance matrix of the image:"<<endl;
	//SImageIO::write_png_file("music1_dist_matrix.png", D_mat, D_mat, D_mat);
	//cout<<"done with distance"<<endl;

	//calculating the scores
	SDoublePlane dist_score(input.rows(),input.cols());
	cout<<"pushing the distance scores.."<<endl;
	double sum = 0;
	for(int i=0; i<input.rows(); i++)
	{
		for(int j=0; j<input.cols(); j++)
		{	
			sum = 0;
			for(int k=0; k<temp.rows(); k++)
			{
				for(int l=0;l<temp.cols(); l++)
				{
					if((i+k) < input.rows() && (j+l)<input.cols())
						sum += template_binary_edgemap[k][l] * D_mat[i+k][j+l];	
				}
			}
			dist_score[i][j] = sum;
		}	
	}

	//cout<<endl<<"printing the values of the score matrix"<<endl;
	//SImageIO::write_png_file("1234.png", dist_score, dist_score, dist_score);
	
	SDoublePlane inverted = normalise(dist_score)*-1 + 255;
	SImageIO::write_png_file("debug.png", inverted, inverted, inverted);
	//dist_score = threshold(dist_score, 300, THRESH_MAX, THRESH_ZERO);
	std::vector<DetectedSymbol> symbols;
	for (int i=0; i<inverted.rows(); i++) {
		for (int j=0; j<inverted.cols(); j++) {
			if (inverted[i][j] < 60) {
				DetectedSymbol s;
				s.row = i-3;
				s.col = j-5;
				s.width = 25;
				s.height = 15;
				s.type = (Type)2;
				s.confidence = 0.7;
				s.pitch = 'A';
				symbols.push_back(s);
			}
		}
	}
	
	
	return symbols;
	
	//SImageIO::write_png_file("BinarytempTrial.png", image, image, image);
	//return input;
}

SDoublePlane get_sobel(SDoublePlane input)
{
	SDoublePlane sobelx_kernel = load_kernel("kernels/sobel3x");
	SDoublePlane sobely_kernel = load_kernel("kernels/sobel3y");
	
	SDoublePlane sobelx = convolve_general(input, sobelx_kernel);
	SDoublePlane sobely = convolve_general(input, sobely_kernel);
	
	SDoublePlane ret = sobelx + sobely;
	SImageIO::write_png_file("Sobel_temp.png", ret, ret, ret);
	return ret;
}


double f_gamma(double val)
{
	if(val != 0)
		return 0;
	return INT_MAX;
}

SDoublePlane dist(SDoublePlane img)
{
	double min_dist = INT_MAX;
	double temp;
	double gamma_val;
	SDoublePlane D(img.rows(), img.cols()); 
	for(int i=0; i<img.rows(); i++)
	{
		for(int j=0; j<img.cols(); j++)
		{	
			min_dist = INT_MAX;
			if(img[i][j]>0)
			{
				D[i][j] = 0;
				continue;
			}

			for(int a=0; a<img.rows(); a++)
			{
				for(int b=0; b<img.cols(); b++)
				{
					if( i==a && j==b)
						continue;
					if(img[a][b] != 0)
						gamma_val = 0;
					else
						gamma_val = INT_MAX;
					temp = gamma_val + sqrt( ((i-a)*(i-a)) + ((j-b)*(j-b))) ;
					if(temp < min_dist)
						min_dist = temp;		
				}
			}
			D[i][j] = min_dist;
		}	
	}

	return D;
}

SDoublePlane convert_to_binary_edgemap(SDoublePlane img)
{
	for (int i=0; i<img.rows(); i++)
	{
		for(int j=0; j<img.cols(); j++)
			if(img[i][j]!=0)
				img[i][j]=1;
	}
	return img;

}
*/
SDoublePlane threshold(const SDoublePlane& input, double val, int low_val, int high_val) {
	SDoublePlane image(input);
	for (int i=0, rows = image.rows(); i< rows; i++) {
		for (int j=0, cols = image.cols(); j < cols; j++) {
			if (image[i][j] < val) {
				if (low_val == THRESH_RETAIN) {
					// unchanged.
				} else if (low_val == THRESH_ZERO) {
					image[i][j] = 0;
				} else if (low_val == THRESH_MAX) {
					image[i][j] = 255;
				} else {
					image[i][j] = low_val;
				}
			} else {
				if (high_val == THRESH_RETAIN) {
					// unchanged.
				} else if (high_val == THRESH_ZERO) {
					image[i][j] = 0;
				} else if (high_val == THRESH_MAX) {
					image[i][j] = 255;
				} else {
					image[i][j] = high_val;
				}
			}
		}
	}
	return image;
}

double image_max(const SDoublePlane& input) {
	double *start = input.data_ptr();
	double *end = start + input.rows()*input.cols();
	double *pos = std::max_element(start, end);
	return (pos == end)? -1.0: *pos;
}

double image_sum(const SDoublePlane& input) {
	double *start = input.data_ptr();
	double *end = start + input.rows()*input.cols();
	return std::accumulate(start, end, 0);
}

SDoublePlane normalise(SDoublePlane input) {
	return input * (255.0/image_max(input));
}

SDoublePlane flipxy(SDoublePlane input) {
	for (int i=0; i<input.rows()/2; i++) {
		for (int j=0; j<input.cols(); j++) {
			//std::cout<<"Flipping: "<<i<<" "<<j<<std::endl;
			double temp = input[input.rows() - i - 1][input.cols() - j - 1];
			input[input.rows() - i - 1][input.cols() - j - 1] = input[i][j];
			input[i][j] = temp;
		}
	}
	return input;
}

SDoublePlane normalise_kernel(const SDoublePlane& input) {
	std::cout<<"Normalising kernel: "<<input.rows()<<","<<input.cols()<<std::endl;
	debug_png("before-normalise-kernel.png", input);
	if (input.cols()%2 == 1 && input.rows()%2 == 1) {
		return input;
	}

	SDoublePlane output(input.rows()|1, input.cols()|1);

	for (int i=0; i<input.rows(); i++) {
		memcpy(output[i], input[i], sizeof(output[i][0]) * input.cols());
	}

	if (input.rows() + 1 == output.rows()) {
		memcpy(output[output.rows()-1], input[input.rows()-1], sizeof(output[0][0]) * input.cols());
	}

	if (input.cols() + 1 == output.cols()) {
		for (int i=0; i<input.rows(); i++) {
			output[i][output.cols()-1] = input[i][input.cols()-1];
		}
		if (input.rows() + 1 == output.rows()) {
			output[output.rows() - 1][output.cols() - 1] = input[input.rows()-1][input.cols()-1];
		}
	}

	debug_png("after-normalise-kernel.png", output);
	return output;
}

SDoublePlane non_maximum_suppression(const SDoublePlane& input) {
	int nrows = input.rows(), ncols = input.cols();
	SDoublePlane output(input);
	for (int i=0; i<nrows; i++) {
		for (int j=0; j<ncols; j++) {
			double pixel = output[i][j];
			if (i < nrows - 1 && pixel < input[i+1][j]) {
				output[i][j] = 0;
			}
			else if (i > 0 && pixel < input[i-1][j]) {
				output[i][j] = 0;
			}
			else if (j < ncols - 1 && pixel < input[i][j+1]) {
				output[i][j] = 0;
			}
			else if (j > 0 && pixel < input[i][j-1]) {
				output[i][j] = 0;
			}
		}
	}
	return output;
}

// Convolve an image with a separable convolution kernel
//
SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
	SDoublePlane output(input.rows(), input.cols());

	output = convolve_general(input, row_filter);
	return convolve_general(output, col_filter);
}

// Convolve an image with a general convolution kernel
//
SDoublePlane convolve_general(const SDoublePlane &input, const SDoublePlane &filter)
{
	//Requires the dimension of the filter to be smaller than the input.
	//Switch the parameters otherwise.
	
	if ((input.rows() - filter.rows()) * (input.cols() - filter.cols()) < 0) {
		throw "Mismatched dimensions.";
	}
	if (input.rows() < filter.rows()) {
		return convolve_general(filter, input);
	}
	
	if (filter.rows()%2 == 0 || filter.cols()%2 == 0){
		throw "Expected an odd number of rows and columns in the filter";
	}
	//From here, we have ensured dimensions of input is larger than the filter.
	
	SDoublePlane output(input.rows(), input.cols());

	int 	filter_rows_num = filter.rows(),
			filter_cols_num = filter.cols(),
			image_rows_num = input.rows(),
			image_cols_num = input.cols();
	int 	start_row = filter.rows()/2,
			start_col = filter.cols()/2,
			end_row = input.rows() - start_row,
			end_col = input.cols() - start_col;

	for (int i = 0; i < image_rows_num; i++) {
		for (int j = 0; j < image_cols_num; j++) {
			int sum = 0;
			for (int p = -start_row; p <= start_row; p++) {
				for (int q = -start_col; q <= start_col; q++) {
					int x = i + p, y = j+q;
					if (0 > x || x >= image_rows_num || 
							0 > y || y >= image_cols_num) {
						if (y < 0) {
							y = -y;
						} else if (y >= image_cols_num) {
							y = 2*image_cols_num - y - 1;
						}

						if (x < 0) {
							x = -x;
						} else if (x >= image_rows_num) {
							x = 2*image_rows_num - x - 1;
						}
					}
					sum += input[x][y] * 
						filter[filter_rows_num - p - start_row - 1]
							  [filter_cols_num - q - start_col - 1];
				}
			}
			output[i][j] = sum;
		}
	}
	
	return output;
}

char get_pitch(const DetectedSymbol& symbol, const StaffDetector& sd) {
	int	nearest_dist = 100000,
		n = -1;
	
	const std::vector<std::vector<int> > &g = sd.groups();
	for (int i=0; i < g.size(); i++) {
		int min_dist = std::min(std::abs(symbol.x - g[i][0]), std::abs(symbol.x - g[i][g[i].size()-1]));
		if (min_dist < nearest_dist) {
			nearest_dist = min_dist;
			n = i;
		}
	}

	bool is_nearer_lower = std::abs(symbol.x - g[n][0]) < std::abs(symbol.x - g[n][g[n].size() - 1]);
	if (is_nearer_lower && n % 2 == 0) { //Lower portion of trebel
		char PITCHES[] = {'E', 'F', 'G', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'A', 'B', 'C', 'D'};
		int steps = 2* (symbol.x - g[n][g[n].size() - 1]) / sd.staff_height();
		steps = (steps + 14)%14;
		return PITCHES[steps];
	}
	if (!is_nearer_lower && n % 2 == 0) { // upper portion of trebel
		char PITCHES[] = {'E', 'F', 'G', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'A', 'B', 'C', 'D'};
		int steps = 2 * (g[n][g[n].size() - 1] - symbol.x) / sd.staff_height();
		steps = (steps + 14)%14;
		return PITCHES[steps];
	}
	return 'A';
}

