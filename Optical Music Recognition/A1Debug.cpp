#include <sstream>

#include "A1Debug.h"
#include "SImageIO.h"

namespace {
	int counter = 1;
}

void debug_png(const char* file, const SDoublePlane& img) {
#if DEBUGGING_A1
	std::stringstream ss;
	ss<<"debug/"<<counter++<<file;
	SImageIO::write_png_file(ss.str().c_str(), img, img, img); 
#endif
}
