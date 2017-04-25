#ifndef __DETECTION_UTILS_H
#define __DETECTION_UTILS_H

#include <string>
#include <vector>

#include "SImage.h"
#include "Symbols.h"


void overlay_rectangle(SDoublePlane& input, int, int, int, int, double, int);

void  write_detection_txt(const std::string &filename, 
				const std::vector<DetectedSymbol> &);

void  write_detection_image(const std::string&, 
				const std::vector<DetectedSymbol> &,
				const SDoublePlane &);

void draw_line(SDoublePlane& R, SDoublePlane& G, SDoublePlane& B, 
			int x1, int y1, int x2, int y2,
			double r, double g, double b, int thickness);
	
#endif
