#ifndef TEMPLATE_DETECTOR_H
#define TEMPLATE_DETECTOR_H

#include <vector>

#include "SImage.h"
#include "Symbols.h"
#include "Config.h"
#include "StaffDetector.h"

class TemplateDetector {
	SDoublePlane mainKernel, invertedKernel;
	Type type;
	Config& config;
public:
	SDoublePlane convolve_result;
	TemplateDetector(const SDoublePlane& kernel, Type, Config&);

	std::vector<DetectedSymbol> find(const SDoublePlane& image, StaffDetector, double threshold);
};

#endif
