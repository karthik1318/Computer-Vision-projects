#ifndef GAUSSBLUR
#define GAUSSBLUR
#include "SImage.h"

class Gaussblur{
SDoublePlane gaussx,gaussy;
public:
	Gaussblur(int);
	SDoublePlane blur(const SDoublePlane &);
};


#endif
