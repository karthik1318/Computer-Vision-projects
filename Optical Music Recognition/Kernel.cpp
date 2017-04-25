#include <fstream>
#include <iostream>

#include "Kernel.h"

SDoublePlane load_kernel(const char* filename) {
	std::ifstream in(filename);
	int rows, cols;
	double ratio;
	if (!(in>>rows>>cols>>ratio)) {
		std::cerr<<"Failed reading kernel: "<<filename<<std::endl;
		throw "Failed to read kernel:";
	}
	SDoublePlane kernel(rows, cols);
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			in>>kernel[i][j];
			kernel[i][j] /= ratio;
		}
	}
	return kernel;
}
