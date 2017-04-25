#include <cmath>
#include <iostream>
#include <climits>

#include "EdgeTemplateDetector.h"
#include "Kernel.h"
#include "utils.h"
#include "SImageIO.h"
#include "A1Debug.h"

namespace {
	SDoublePlane convert_to_binary_edgemap(SDoublePlane img) {
		for (int i=0; i<img.rows(); i++) {   
			for(int j=0; j<img.cols(); j++) {
				if(img[i][j]!=0)
					img[i][j]=1;
			}
		}
		return img;
	}

	SDoublePlane dist(SDoublePlane img) {
		double min_dist = INT_MAX;
		double temp;
		double gamma_val;
		SDoublePlane D(img.rows(), img.cols());
		for(int i=0; i<img.rows(); i++) {
			for(int j=0; j<img.cols(); j++) {
				//std::cout<<"Processing Dist Map: "<<i<<" "<<j<<std::endl;
				min_dist = INT_MAX;
				if(img[i][j]>0) {
					D[i][j] = 0;
					continue;
				}

				for(int a=0; a<img.rows(); a++) {
					for(int b=0; b<img.cols(); b++)
					{
						if( i==a && j==b)
							continue;
						if(img[a][b] != 0)
							gamma_val = 0;
						else
							gamma_val = INT_MAX;
						temp = gamma_val + std::sqrt( ((i-a)*(i-a)) + ((j-b)*(j-b))) ;
						if(temp < min_dist)
							min_dist = temp;    
					}
				}
				D[i][j] = min_dist;
			}
		}   

		return D;
	}
	double f_gamma(double val) {
		if(val != 0)
			return 0;
		return INT_MAX;
	}

	SDoublePlane get_sobel(SDoublePlane input) {
		SDoublePlane sobelx_kernel = load_kernel("kernels/sobel3x");
		SDoublePlane sobely_kernel = load_kernel("kernels/sobel3y");

		SDoublePlane sobelx = convolve_general(input, sobelx_kernel);
		SDoublePlane sobely = convolve_general(input, sobely_kernel);

		SDoublePlane ret = sobelx + sobely;
		debug_png("Sobel_temp.png", ret);
		return ret;
	}

	SDoublePlane correlate(SDoublePlane input, SDoublePlane temp) {	
		SDoublePlane dist_score(input.rows(),input.cols());	
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
							sum += temp[k][l] * input[i+k][j+l];
					}
				}
				dist_score[i][j] = sum;
			}
		}
		return dist_score;
	}
}
	


EdgeTemplateDetector::EdgeTemplateDetector(const SDoublePlane& img): image(img) {
	SDoublePlane image_binary_edgemap = convert_to_binary_edgemap(get_sobel(image));
	
	//calculating the the distance matrix D
	std::cout<<"starting distance calculator.."<<std::endl;
	D_mat = dist(image_binary_edgemap);
	//SImageIO::write_png_file("music1_sobel_dist_matrix.png", D_mat, D_mat, D_mat);
	//std::cout<<"wrote the distance matrix to  -  music1_dist_matrix"<<std::endl;

	//reading directly from the dist_matrix image saved
	//SDoublePlane D_mat = SImageIO::read_png_file("music1_dist_matrix.png");

	
	//calculating the scores
	
	
}

std::vector<DetectedSymbol> EdgeTemplateDetector::find(const SDoublePlane& kernel, StaffDetector sd, Type t, double threshold) {
	std::cout<<"pushing the distance scores.."<<std::endl;
	
	//SDoublePlane temp = SImageIO::read_png_file(kernel);
	SDoublePlane template_binary_edgemap1 = convert_to_binary_edgemap(get_sobel(kernel));

	SDoublePlane dist_score = correlate(D_mat,template_binary_edgemap1);
	SDoublePlane inverted = normalise(dist_score)*-1 + 255;
	
	//SImageIO::write_png_file("debug.png", inverted, inverted, inverted);
	
	//dist_score = threshold(dist_score, 300, THRESH_MAX, THRESH_ZERO);
	std::vector<DetectedSymbol> symbols;
	for (int i=0; i<inverted.rows(); i++) {
		for (int j=0; j<inverted.cols(); j++) {
			if (inverted[i][j] < 20) {
				DetectedSymbol s;
				s.row = i-3;
				s.col = j-5;
				s.width = 25;
				s.height = 20;
				s.type = t;
				s.confidence = (20 -inverted[i][j])/(20);
				s.pitch = get_pitch(s, sd);
				symbols.push_back(s);
			}
		}
	}
	return symbols;
}
