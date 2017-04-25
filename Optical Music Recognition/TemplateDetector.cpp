#include "TemplateDetector.h"
#include "utils.h"
#include "SImageIO.h"
#include "A1Debug.h"

TemplateDetector::TemplateDetector(const SDoublePlane& kernel, Type type, Config& c):type(type), config(c) {
	mainKernel = threshold(flipxy(normalise_kernel(kernel)), 195, THRESH_ZERO, THRESH_MAX);//150
	invertedKernel = mainKernel*-1 + 255;
}

std::vector<DetectedSymbol> TemplateDetector::find(const SDoublePlane& image, StaffDetector sd, double thresh) {
	//SImageIO::write_png_file("_main.png", mainKernel, mainKernel, mainKernel);
	//SImageIO::write_png_file("_inverted.png", invertedKernel, invertedKernel, invertedKernel);
	SDoublePlane invertedImage = image* -1 + 255;
	//SImageIO::write_png_file("_invertedImage.png", invertedImage, invertedImage, invertedImage);
	SDoublePlane output1 = convolve_general(image, mainKernel);
	SDoublePlane output2 = convolve_general(invertedImage, invertedKernel);
	SDoublePlane output = normalise(output1 + output2);
	this->convolve_result = output;

	debug_png("template.png", output);
	output = non_maximum_suppression(output);
	debug_png("template_nma.png", output);
	
	output = threshold(output, thresh, THRESH_ZERO, THRESH_RETAIN);

	debug_png("template_nma_thresh.png", output);

	std::vector<DetectedSymbol> symbols;
	for (int i=0; i<output.rows(); i++) {
		for (int j=0; j<output.cols(); j++) {
			if (output[i][j] >= thresh) {
				DetectedSymbol s;
				s.row = i - mainKernel.rows()/2 - 3;
				s.col = j - mainKernel.cols()/2 - 3;
				s.x = i;
				s.y = j;
				s.width = mainKernel.cols() + 6;
				s.height = mainKernel.rows() + 6;
				s.type = type;
				s.confidence = (output[i][j] - (double)thresh) / (255.0 - thresh);
				s.pitch = get_pitch(s, sd);
				symbols.push_back(s);
			}
		}
	}
	return symbols;
}
