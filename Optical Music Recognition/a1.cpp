#include <cmath>
#include <iostream>

#include "A1Debug.h"
#include "DrawText.h"
#include "SImage.h"
#include "SImageIO.h"
#include "TemplateDetector.h"
#include "EdgeTemplateDetector.h"
#include "Kernel.h"
#include "Hough.h"
#include "Canny.h"
#include "detection_utils.h"
#include "Gaussblur.h"
#include "Config.h"
#include "StaffDetector.h"
#include "ImageResize.h"

int process(const char* filename) {
	const clock_t start = clock();
	Config config("config.txt");
	SDoublePlane input_image= SImageIO::read_png_file(filename);
	Gaussblur gauss(5);

	SDoublePlane smoothed_image = gauss.blur(input_image);
	debug_png("gauss.png", smoothed_image);
	
	SDoublePlane canny_image = canny(input_image,
						config.get<double>("canny.low_thresh"), 
						config.get<double>("canny.high_thresh"));
	debug_png("canny.png", canny_image);

	HoughLinesDetector hough(config);
	std::vector<int> lines = hough.find(canny_image, input_image);
	StaffDetector sd(lines);

	SDoublePlane template1 = get_template(1, sd.staff_height());
	SDoublePlane template2 = get_template(2, sd.staff_height());
	SDoublePlane template3 = get_template(3, sd.staff_height());

	std::vector<DetectedSymbol> detected;
	TemplateDetector detector1(template1, NOTEHEAD, config);
	std::vector<DetectedSymbol> symbols1 = 
					detector1.find(smoothed_image, sd, config.get<double>("template1.thresh"));
	detected.insert(detected.end(), symbols1.begin(), symbols1.end());
	SImageIO::write_png_file("scores4-template1.png", detector1.convolve_result, 
				detector1.convolve_result, detector1.convolve_result);

	TemplateDetector detector2(template2, QUARTERREST, config);
	std::vector<DetectedSymbol> symbols2 =
					detector2.find(smoothed_image, sd, config.get<double>("template2.thresh"));
	detected.insert(detected.end(), symbols2.begin(), symbols2.end());
	SImageIO::write_png_file("scores4-template2.png", detector2.convolve_result, 
				detector2.convolve_result, detector2.convolve_result);

	TemplateDetector detector3(template3, EIGHTHREST, config);
	std::vector<DetectedSymbol> symbols3 =
	detector3.find(smoothed_image, sd, config.get<double>("template3.thresh"));
	detected.insert(detected.end(), symbols3.begin(), symbols3.end());
	SImageIO::write_png_file("scores4-template3.png", detector3.convolve_result, 
				detector3.convolve_result, detector3.convolve_result);
	
	write_detection_image("detected4.png", detected, input_image);

	SImageIO::write_png_file("edges.png",canny_image, canny_image, canny_image);

	//calculating the template matching score using edgemaps method
	std::vector<DetectedSymbol> detected1;

	EdgeTemplateDetector edge = EdgeTemplateDetector(input_image);
	SDoublePlane temp1 = SImageIO::read_png_file("template1.png");
	std::vector<DetectedSymbol> symbols4 = edge.find(temp1, sd, NOTEHEAD, 0);
	detected1.insert(detected1.end(), symbols4.begin(), symbols4.end());

	SDoublePlane temp2 = SImageIO::read_png_file("template2.png");
	std::vector<DetectedSymbol> symbols5 = edge.find(temp2, sd, QUARTERREST, 0);
	detected1.insert(detected1.end(), symbols5.begin(), symbols5.end());

	SDoublePlane temp3 = SImageIO::read_png_file("template3.png");	
	std::vector<DetectedSymbol> symbols6 = edge.find(temp3, sd, EIGHTHREST, 0);
	detected1.insert(detected1.end(), symbols6.begin(), symbols6.end());

	write_detection_image("detected5.png", detected1, input_image);

	std::vector<DetectedSymbol> result = detected;

	write_detection_image("detected7.png", result, input_image);
	write_detection_txt("detected7.txt", result);

	write_detection_txt("detected.txt", result);
	write_detection_image("detected.png", result, input_image);

	std::cout<<"Time taken: "<<float( clock() - start)/CLOCKS_PER_SEC<<std::endl;
	return 0;
}
//
// This main file just outputs a few test images. You'll want to change it to do 
//  something more interesting!
//
int main(int argc, char *argv[])
{
	if(!(argc == 2))
	{
		std::cerr << "Usage: " << argv[0] << " input_image" << std::endl;
		return 1;
	}
	try {
		return process(argv[1]);
	} catch (const char* err) {
		std::cerr<<err<<std::endl;
		return 1;
	}
}
