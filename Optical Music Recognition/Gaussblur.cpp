#include "Gaussblur.h"
#include "Canny.h"
#include "Kernel.h"
#include "utils.h"


Gaussblur::Gaussblur(int size){
	if(size==5){
		gaussx = load_kernel("kernels/gauss5x");
		gaussy = load_kernel("kernels/gauss5y");
	} else { //The same kernels for now
		gaussx = load_kernel("kernels/gauss5x");
		gaussy = load_kernel("kernels/gauss5y");
	}
}

SDoublePlane Gaussblur::blur(const SDoublePlane &input) {
	return convolve_separable(input,gaussx,gaussy);	
}



