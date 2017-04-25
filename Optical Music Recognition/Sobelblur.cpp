#include "Sobelblur.h"
#include "Kernel.h"
#include "utils.h"

namespace {
	SDoublePlane sobelx1=load_kernel("kernels/sobelx1");
	SDoublePlane sobelx2=load_kernel("kernels/sobelx2");
	SDoublePlane sobely1=load_kernel("kernels/sobely1");
	SDoublePlane sobely2=load_kernel("kernels/sobely2");
}

SDoublePlane Sobelblur::blur(const SDoublePlane &input,bool xdirection) {
	if(xdirection)	
		return convolve_separable(input,sobelx1,sobelx2);	
	else
		return convolve_separable(input,sobely1,sobely2);		
}



