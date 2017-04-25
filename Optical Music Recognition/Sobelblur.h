#ifndef SOBELBLUR
#define SOBELBLUR
#include "SImage.h"


class Sobelblur{
public:
	SDoublePlane blur(const SDoublePlane &,bool);
};

#endif
