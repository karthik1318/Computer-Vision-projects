// Skeleton code for B657 A4 Part 1.
// D. Crandall
//
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <CImg.h>
#include <assert.h>

using namespace cimg_library;
using namespace std;

int main(int argc, char *argv[])
{
  if(argc != 3)
    {
      cerr << "usage: " << argv[0] << " image_file disp_file" << endl;
      return 1;
    }

  string input_filename1 = argv[1], input_filename2 = argv[2];

  // read in images and gt
  CImg<double> image_rgb(input_filename1.c_str());
  CImg<double> image_disp(input_filename2.c_str());
  CImg<double> image_right(image_rgb._width,image_rgb._height,1,3);
  int shift;
  for(int i=0;i<image_rgb.width();i++){
    for(int j=0;j<image_rgb.height();j++){
     shift = (image_disp(i,j,0)) / 30 ;
      if(i-shift<image_rgb._width && i-shift >=0){
        image_right(i,j,0,0)= image_rgb(i-shift,j,0,0);
        image_right(i,j,0,1)= image_rgb(i-shift,j,0,1);
        image_right(i,j,0,2)= image_rgb(i-shift,j,0,2);
        }
      }
  }
  
  image_right.save_png((input_filename1 + "-right.png").c_str());
  CImg<double> image_result = image_rgb;
  image_result.fill(0);
  for(int i=0;i<image_rgb.width();i++){
    for(int j=0;j<image_rgb.height();j++){
        image_result(i,j,0,0)= image_rgb(i,j,0,0);
        image_result(i,j,0,1)= image_right(i,j,0,1);
        image_result(i,j,0,2)= image_right(i,j,0,2);
    }
  }
  image_result.get_normalize(0,255).save((input_filename1 + "-stereogram.png").c_str());

  return 0;
}
