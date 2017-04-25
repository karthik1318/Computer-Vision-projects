//
//  DLib: A simple image processing library.
//
//  David Crandall, 2003-2005
//  crandall@cs.cornell.edu
//
//  Please do not redistribute this code.
//
//
//
//
#ifndef __DRAWTEXT_H_
#define __DRAWTEXT_H_

#include "SImage.h"

void draw_text(SDoublePlane &img, const char *str, int row, int col, int value, int scale);

#endif
