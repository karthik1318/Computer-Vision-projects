#include <sstream>
#include <string>

#include "ImageResize.h"
#include "SImageIO.h"

SDoublePlane get_template(int template_num, double staff_height) {
	std::stringstream ss;
	int res = ((int)(staff_height * 5 / (11.0) + 0.5) + 5) * 10;
	ss<<"templates/template"<<template_num<<"-"<<res<<".png";
	std::cout<<"Loading Template: "<<ss.str()<<std::endl;
	return SImageIO::read_png_file(ss.str().c_str());
}
