#ifndef __EDGE_TEMPLATE_DETECTOR_H__
#define __EDGE_TEMPLATE_DETECTOR_H__

#include <vector>

#include "Symbols.h"
#include "SImage.h"
#include "StaffDetector.h"

class EdgeTemplateDetector {
	SDoublePlane image;
	SDoublePlane edge_map;

	SDoublePlane D_mat;
public:
	EdgeTemplateDetector(const SDoublePlane&);
	std::vector<DetectedSymbol> find(const SDoublePlane&, StaffDetector sd, Type, double threshold);
};

#endif
