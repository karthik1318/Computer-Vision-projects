#include <cmath>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "Hough.h"
#include "SImageIO.h"
#include "A1Debug.h"
#include "Config.h"
#include "detection_utils.h"

Line::Line(double _x1, double _y1, double _x2, double _y2): 
						x1(_x1), y1(_y1), x2(_x2), y2(_y2) {
	double deltaX = x2 - x1, deltaY = y2 -  y1;
	_vertical = std::abs(deltaX) < std::abs(deltaY);
	_slopeI = _slope = 0.0;
	if (_vertical) {
		_slopeI = deltaY / deltaX;
	} else {
		_slope = deltaX / deltaY;
	}
	_intercept = _vertical? x1 - y1 * _slopeI : y1 - x1 * _slope;
}

double Line::slope() {
	return _vertical? _slopeI: _slope;
}

double Line::yintercept() {
	return _intercept;
}

double Line::xintercept() {
	return 0;
}

bool Line::vertical() {
	return _vertical;
}

HoughLinesDetector::HoughLinesDetector(Config& config): config(config) {
	
}

std::vector<int> HoughLinesDetector::find(const SDoublePlane& input, const SDoublePlane& orig) {
	debug_png("hough_input.png", input);
	int rows = input.rows(), cols = input.cols();

	int angle_bins = config.get<int>("hough.angle_bins");
	double max_angle = M_PI;
	int max_r = 2*(int)(std::sqrt(rows*rows + cols*cols)+1) + 2;
	int r_bins = std::min(max_r, config.get<int>("hough.r_bins_max"));
	r_bins = max_r;


	double min_line_angle = config.get<double>("hough.min_angle");
	double max_line_angle = config.get<double>("hough.max_angle");

	SDoublePlane hough(r_bins, angle_bins);

	//memset zero on a double array: Thug life! :P
	std::memset(hough.data_ptr(), 0, hough.rows() * hough.cols() * sizeof(double));

	for (int angle_step = 0; angle_step < angle_bins; angle_step++) {
		double angle = max_angle * angle_step / (float)angle_bins;
		double deg_angle = angle * 180.0 / M_PI;
		if (deg_angle < min_line_angle || deg_angle > max_line_angle)
			continue;
		for (int i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {
				if (input[i][j] > 254) {
					double r = i * std::cos(angle) + j * std::sin(angle);

					int row = (int)((r / (max_r/2.0) + 1.0)*r_bins/2.0);
					int col = angle_step;
					hough[row][col] += 1;
				}
			}
		}
	}
	hough = normalise(hough);
	debug_png("hough_output.png", hough);

	hough = threshold(hough, config.get<double>("hough.thresh"), THRESH_ZERO, THRESH_RETAIN);
	//hough = non_maximum_suppression(hough);
	
	double max_line_length = std::sqrt(rows*rows + cols*cols);
	std::vector<int> lines;
	for (int i=0; i<hough.rows(); i++) {
		for (int j=0; j<hough.cols(); j++) {
			if (!hough[i][j])
				continue;
			double r = (2.0 * i / r_bins - 1.0) * max_r / 2.0;
			double theta = max_angle * j / angle_bins;
			std::cout<<"Found a line with parameter: r="<<(int)(r+0.5)<<", angle="<<theta<<std::endl;

			lines.push_back(int(r+0.5));
		}
	}
	
	// Group the lines! Max - min = config["houfh.group_max_delta"]
	std::sort(lines.begin(), lines.end());
	std::vector<std::vector<int> > groups;
	int max_delta = config.get<double>("hough.group_max_delta");
	
	for (std::vector<int>::iterator it = lines.begin(); it != lines.end();) {
		std::vector<int> group;
		int curMin = *it;
		group.push_back(*it);

		while (++it != lines.end()) {
			if (*it - curMin <= max_delta) {
				group.push_back(*it);
			} else {
				break;
			}
		}

		groups.push_back(group);
	}

	std::vector<int> result;
	for (std::vector<std::vector<int> >::iterator it = groups.begin(); it != groups.end(); it++) {
		result.push_back(std::accumulate(it->begin(), it->end(), 0)/it->size()); //Average in group.
	}
	std::cout<<"Done with averaging.."<<std::endl;

	SDoublePlane paintR(orig), paintG(orig), paintB(orig);
	
	for (std::vector<int>::iterator it = result.begin(); it != result.end(); it++) {
		std::cout<<"Emitting a line with offset: "<<*it<<std::endl;
		draw_line(paintR, paintG, paintB, *it, 0, *it, input.cols()-1, 0, 0, 255, 3);
	}
	std::cout<<"Done with painting.."<<std::endl;

	SImageIO::write_png_file("staves.png", paintR, paintG, paintB);

	return result;
}

