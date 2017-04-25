#include<cmath>
#include <iostream>
#include "Sobelblur.h"
#include "Canny.h"
#include "utils.h"
#include "Kernel.h"
#include "SImageIO.h"
#include "A1Debug.h"

#define DIR_NORTH 0
#define DIR_NORTH_EAST 1
#define DIR_EAST 2
#define DIR_NORTH_WEST 3

#define BETWEEN(X,Y,Z) (((X) <= (Y)) && ((Y) <= (Z)))
#define MAXIMA(X,Y,Z) (((x) < (Y)) && ((Y) > (Z)))
namespace {
	Sobelblur sobel;
}

SDoublePlane canny(const SDoublePlane& image, double low_thresh, double high_thresh) {
	/*
	 * Reference Used:http://www.inf.u-szeged.hu/ssip/2003/lectures/chetverikov/edge_detection.pdf
	 * Slides #16-#21
	 */

	SDoublePlane sobelx = sobel.blur(image,true);
	SDoublePlane sobely = sobel.blur(image,false);
	SDoublePlane magnitude(image.rows(), image.cols());
	SDoublePlane output = image;
	_DTwoDimArray<int> angle(image.rows(), image.cols());

	debug_png("sobel.png", sobelx + sobely);

	for (int i=0; i<image.rows(); i++) {
		for (int j=0; j<image.cols(); j++) {
			magnitude[i][j] = std::sqrt(sobelx[i][j]*sobelx[i][j] + 
										sobely[i][j]*sobely[i][j]);
			double ang = std::atan2(sobely[i][j], sobely[i][j]) * 180.0;
			if (BETWEEN(-22.5,ang,22.5) || (BETWEEN(157.5,ang,180) && BETWEEN(-180,ang,-157.5)))
				angle[i][j] = DIR_EAST;
			else if (BETWEEN(22.5,ang,67.5) || BETWEEN(-157.5,ang,-112.5))
				angle[i][j] = DIR_NORTH_EAST;
			else if (BETWEEN(67.5,ang,112.5) || BETWEEN(-112.5,ang,-67.5))
				angle[i][j] = DIR_NORTH;
			else if (BETWEEN(112.5,ang,157.5) || BETWEEN(-67.5,ang,-22.5))
				angle[i][j] = DIR_NORTH_WEST;

			if (magnitude[i][j] < low_thresh) { 
				//std::cout<<"Suppressed because lower than threshold: "<<i<<","<<j<<std::endl;
				output[i][j] = 0;
			} else {
				output[i][j] = 255;
			}
		}
	}

	debug_png("canny_magnitude.png", magnitude);

	for (int i=0; i<output.rows(); i++) {
		for (int j=0; j<output.cols(); j++) {
			if (output[i][j] < 1.0) {
				output[i][j] = 0;
				continue;
			}
			if (angle[i][j] == DIR_NORTH) {
				if (i > 0 && magnitude[i-1][j] > magnitude[i][j]) {
					output[i][j] = 0;
				}
				else if (i < image.rows() - 1 && magnitude[i+1][j] > magnitude[i][j]) {
					output[i][j] = 0;
				}
				else {
					//std::cout<<"Found maximum at: "<<i<<","<<j<<std::endl;
				}
			}
			else if (angle[i][j] == DIR_NORTH_EAST) {
				if (i > 0 && j < image.cols() - 1 && magnitude[i-1][j+1] > magnitude[i][j]) {
					output[i][j] = 0;
				}
				else if (i < image.rows() - 1 && j > 0 && magnitude[i+1][j-1] > magnitude[i][j]) {
					output[i][j] = 0;
				}else {
					//std::cout<<"Found maximum at: "<<i<<","<<j<<std::endl;
				}

			}
			else if (angle[i][j] == DIR_EAST) {
				if (j < image.cols() - 1 && magnitude[i][j+1] > magnitude[i][j]) {
					output[i][j] = 0;
				}
				else if (j > 0 && magnitude[i][j-1] > magnitude[i][j]) {
					output[i][j] = 0;
				}else {
					//std::cout<<"Found maximum at: "<<i<<","<<j<<std::endl;
				}

			}
			else if (angle[i][j] == DIR_NORTH_WEST) {
				if (i > 0 && j > 0 && magnitude[i-1][j-1] > magnitude[i][j]) {
					output[i][j] = 0;
				}
				else if (i < image.rows() - 1 && j < image.cols() - 1 && magnitude[i+1][j+1] < magnitude[i][j]) {
					output[i][j] = 0;
				}else {
					//std::cout<<"Found maximum at: "<<i<<","<<j<<std::endl;
				}

			}
		}
	}

	std::cout<<"Suppressed pixel count: "<<image_sum(threshold(image + output*-1,-1,THRESH_RETAIN, 1))<<std::endl;

	return output;
	
}
