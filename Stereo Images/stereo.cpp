// Skeleton code for B657 A4 Part 3.
// D. Crandall
//
// Run like this, for example:
//   ./stereo part3/Aloe/view1.png part3/Aloe/view5.png part3/Aloe/gt.png
// and output files will appear in part3/Aloe
//
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <CImg.h>
#include <assert.h>
#include <list>
#include <iterator>
#include <limits>

using namespace cimg_library;
using namespace std;

double sqr(double a) { return a*a; }

// This code may or may not be helpful. :) It computes a 
//  disparity map by looking for best correspondences for each
//  window independently (no MRF).
//
class Point
{
public:
  Point() {}
  Point(int _col, int _row) : row(_row), col(_col) {}
  int row, col;
};

enum Direction {TOP, RIGHT, BOTTOM, LEFT};

class LabelCost {
public:
    vector< CImg<double> > costs;
    LabelCost(int width, int height) {
        for (int i = 0 ; i < 3 ; i++) {
            CImg<double> cost(width, height,1,1);
            costs.push_back(cost);
        }
    }
    double getCost(int label, int i, int j) {
        CImg<double> cost = costs[label];
        return cost(j,i,0,0);
    }
    void setCost(int label, int i, int j, double val) {
        CImg<double> cost = costs[label];
        cost(j, i, 0, 0) = val;
    }
};


LabelCost *labelCost;

// Calculating cost
CImg<double> assign_label(const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg , double fmean[], double fvariance[])
{
    double fixed_cost=40000;
    CImg<double> L(img.width(),img.height(),1,1);
    
    labelCost = new LabelCost(img.width(), img.height());
    
    CImg<double> cost(img.width(),img.height(),1,1);
    L.fill(255);
    // Assigning costs of seeds
    for(int i=0;i< fg.size();i++)
    {
        cost(fg[i].col,fg[i].row,0,0)=0;
        L(fg[i].col,fg[i].row,0,0)=1;
    }
    for(int i=0;i< bg.size();i++)
    {
        cost(bg[i].col,bg[i].row,0,0)=0;
        L(bg[i].col,bg[i].row,0,0)=0;
    }
    cout << "Inside Assign Label" << endl;
    // Finding cost and label for each pixel in image
    for(int i=0;i<img.height();i++) {
        for(int j=0;j<img.width();j++)
        {
            if(L(j,i,0,0)==255)
            {
                cout<<"Started finding cost for each pixel"<<endl;
                double first_term = (1/(sqrt((2*3.14)*(2*3.14)*(2*3.14))*fvariance[0]*fvariance[1]*fvariance[2]));
                double red_term= (-0.5 * (1/fvariance[0]*fvariance[0])) *((img(j,i,0,0)-fmean[0])*(img(j,i,0,0)-fmean[0]));
                double green_term= (-0.5 * (1/fvariance[1]*fvariance[1])) *((img(j,i,0,1)-fmean[1])*(img(j,i,0,1)-fmean[1]));
                double blue_term= (-0.5 * (1/fvariance[2]*fvariance[2])) *((img(j,i,0,2)-fmean[2])*(img(j,i,0,2)-fmean[2]));
                double gaussian_cost = -log(first_term) * -1* (red_term + green_term + blue_term);
                //cout <<-log(first_term) <<" "<<red_term <<" "<<green_term <<" "<<blue_term <<" "<< gaussian_cost<<  endl;
                cost(j,i,0,0) = fixed_cost < gaussian_cost ? fixed_cost:gaussian_cost;
                L(j,i,0,0) = fixed_cost < gaussian_cost ? 0:1;
                cout << "assigning label cost" <<endl;
                labelCost->setCost(0, i, j, fixed_cost/8000);
                labelCost->setCost(1, i, j, gaussian_cost/8000);
            }
        }
    }
    cout << "Assign Label Completed" << endl;
    return L;
}

double cost_function (const CImg<double> &img1, const CImg<double> &img2, int i, int j, int l){
    int window_size=5;
    double cost = 0;
	    for(int ii = max(i-window_size, 0); ii <= min(i+window_size, img1.height()-1); ii++)
	      for(int jj = max(j-window_size, 0); jj <= min(j+window_size, img1.width()-1); jj++){
			  cost += sqr(img1(min(jj+l, img1.width()-1), ii) - img2(jj, ii));
		  }
		return cost;
}



class Message {
public:
    vector<double> messages;
    Message() {
        for(int i = 0 ; i < 4 ; i++) {
            messages.push_back(0.0);
        }
    }

    double getMessage(Direction dir) {
        switch (dir) {
            case TOP:
                return messages[TOP];

            case RIGHT:
                return messages[RIGHT];

            case BOTTOM:
                return messages[BOTTOM];

            case LEFT:
                return messages[LEFT];
        }

        return 0;
    }

    void setMessage(Direction dir, double msg) {
        
        switch (dir) {
            case TOP:
                messages[TOP] = msg;
                
                break;

            case RIGHT:
                messages[RIGHT] = msg;
                break;

            case BOTTOM:
                messages[BOTTOM] = msg;
                break;
                
            case LEFT:
                messages[LEFT] = msg;
                break;
        }
    }

    double getMessageSum(Direction dir) {
        double sum;

        for (int i = 0 ; i < 4 ; i++) {
            if (i != dir)
                sum += messages[i];
        }

        return sum;
    }

    double getBeliefSum() {
        double sum;

        for (int i = 0 ; i < 4 ; i++) {
            sum += messages[i];
        }

        return sum;
    }
};

class Pixel {
public:
    vector<Message> labels;
    Pixel() {
        for(int i = 0 ; i < 2 ; i++) {
            Message m;
            labels.push_back(m);
        }
    }
    Message* getLabel(int label) {
        return &labels[label];
    }
};


class Pixels {
public:
    int row, col;
    
    vector< vector<Pixel> > pixels;
    
    Pixels(int i, int j) {
        for (int k = 0 ; k < i ; k++) {
            Pixel p;
            pixels.push_back(vector<Pixel>(j, p));
        }
    }
    
    
    Pixel* getPixel(int i, int j) {
        row = i;
        col = j;

        if ((i < 0 || i >= pixels.size()) || (j < 0 || j >= pixels[0].size()))
            return NULL;

        return &pixels[i][j];
    }
    
    Pixel* getNext(Direction direction) {
        switch(direction) {
            case TOP:
                return &pixels[row + 1][col];

            case RIGHT:
                return &pixels[row][col + 1];

            case BOTTOM:
                return &pixels[row - 1][col];

            case LEFT:
                return &pixels[row][col - 1];
        }
        return NULL;
    }
    
    bool hasNext(Direction direction) {
        switch(direction) {
            case TOP:
                return row + 1 < pixels.size();

            case RIGHT:
                return col + 1 < pixels[0].size();

            case BOTTOM:
                return row - 1 >= 0;

            case LEFT:
                return col - 1 >= 0;
        }
        return false;
    }
};


double smoothness_function(int l1, int l2){
    if (l1 == l2)
        return 0;
    else
        return 1;
}

double createEnergy(const CImg<double> &img1, const CImg<double> &img2, Pixel *pixel, int l1, int i, int j, int dir) {
    double energy1 ;
    double energy2 ;
    double return_energy;
    
    for (int l2 = 0 ; l2 < 2 ; l2++) {
        double cost_value = cost_function(img1,img2, i, j, l2);
        double messagesum = pixel->getLabel(l2)->getMessageSum((Direction) dir);
        double smoothness = smoothness_function(l1, l2);
        if (l2 == 0)
        {
            energy1 = cost_value + messagesum + smoothness;
            cout<< "This is the energy update for label 0"<<energy1<<endl;
        }
        if (l2 == 1)
        {
            energy2 = cost_value + messagesum + smoothness;
            cout<< "This is the energy update for label 1"<<energy2<<endl;
        }

    }
    return_energy = min(energy1,energy2);
    cout<<"This is after for loop"<<return_energy<<endl;
    return return_energy;
}


CImg<double> naive_stereo(const CImg<double> &input1, const CImg<double> &input2, int window_size, int max_disp)
{  
  CImg<double> result(input1.width(), input1.height());
  for(int i=0; i<input1.height(); i++)
    for(int j=0; j<input1.width(); j++)
      {
	pair<int, double> best_disp(0, INFINITY);

	for (int d=0; d < max_disp; d++)
	  {
	    double cost = 0;
	    for(int ii = max(i-window_size, 0); ii <= min(i+window_size, input1.height()-1); ii++)
	      for(int jj = max(j-window_size, 0); jj <= min(j+window_size, input1.width()-1); jj++)
		cost += sqr(input1(min(jj+d, input1.width()-1), ii) - input2(jj, ii));

	    if(cost < best_disp.second)
	      best_disp = make_pair(d, cost);
	  }
	result(j,i) = best_disp.first;
      }

  return result;
}



// implement this!
//  this placeholder just returns a random disparity map
//
CImg<double> mrf_stereo(const CImg<double> &img1, const CImg<double> &img2)
{
  Pixels prev(img1.height(), img1.width());
  Pixels curr(img1.height(), img1.width());
  for (int itr = 1 ; itr <= 1 ; itr++) {
        
        for (int i = 0 ; i < img1.height() ; i++) {
            for (int j = 0 ; j < img1.width() ; j++) {
                Pixel *currentPixel = curr.getPixel(i, j);

                for (int dir = 0 ; dir < 4 ; dir++) {
                    if (!curr.hasNext((Direction) dir))
                        continue;
                    
                    Pixel *nextPixel = curr.getNext((Direction) dir);

                    Pixel *currentPixelPreItr = prev.getPixel(i, j);

                    if (nextPixel != NULL) {
                        
                        for (int l = 0 ; l < 2 ; l++) {
                            double energy = createEnergy(img1, img2, currentPixelPreItr, l, i, j, dir);
							energy=energy/100;
                            cout<<"The energy is"<<":"<<energy;
                            nextPixel->getLabel(l)->setMessage((Direction) ((dir + 2) % 4), energy);
                            
                            //cout<<"this is message" << nextPixel->getLabel(l)->getMessage((Direction) ((dir + 2) % 4)) << endl;
                        }
                    }
                }
            }
        }

        prev = curr;
    }

    CImg<double> result(img1.width(), img1.height());

    for (int i = 0 ; i < img1.height() ; i++) {
        cout<<"Labelling has started"<<endl;
        for (int j = 0; j < img1.width(); j++) {
            //int intLabel = 0;
            int label = 0;
            double temp_0;
            double temp_1;
            
            //double max = numeric_limits<double>::max();
            double sum = 10000000;

            for (int l = 0 ; l < 2 ; l++) {
                if (l==0){
                    temp_0 = cost_function(img1,img2, i, j, l) + curr.getPixel(i, j)->getLabel(l)->getBeliefSum();
                }
                if (l==1) {
                    temp_1 = cost_function(img1,img2, i, j, l) + curr.getPixel(i, j)->getLabel(l)->getBeliefSum();

                }
                cout<<"current Comparison label0 -->"<<temp_0<<"current label1 -->"<<temp_1<<"Current label"<<min(temp_0, temp_1)<<endl;

            }
            
            if (temp_0<temp_1)
            {
                label =0 ;
            }
            
            else{
                label = 1;
            }
            result(j, i, 0, 0) = 255*label;
            cout<<"The label is: " << result(j, i, 0, 0)<<endl;
        }
        
        cout << endl;
    }
    result.get_normalize(0,255).save_png("test.png");
    return result;
}




int main(int argc, char *argv[])
{
  if(argc != 4 && argc != 3)
    {
      cerr << "usage: " << argv[0] << " image_file1 image_file2 [gt_file]" << endl;
      return 1;
    }

  string input_filename1 = argv[1], input_filename2 = argv[2];
  string gt_filename;
  if(argc == 4)
    gt_filename = argv[3];

  // read in images and gt
  CImg<double> image1(input_filename1.c_str());
  CImg<double> image2(input_filename2.c_str());
  CImg<double> gt;

  if(gt_filename != "")
  {
    gt = CImg<double>(gt_filename.c_str());

    // gt maps are scaled by a factor of 3, undo this...
    for(int i=0; i<gt.height(); i++)
      for(int j=0; j<gt.width(); j++)
        gt(j,i) = gt(j,i) / 3.0;
  }
  
  // do naive stereo (matching only, no MRF)
  CImg<double> naive_disp = naive_stereo(image1, image2, 5, 50);
  naive_disp.get_normalize(0,255).save((input_filename1 + "-disp_naive.png").c_str());

  // do stereo using mrf
  CImg<double> mrf_disp = mrf_stereo(image1, image2);
  mrf_disp.get_normalize(0,255).save((input_filename1 + "-disp_mrf.png").c_str());

  // Measure error with respect to ground truth, if we have it...
  if(gt_filename != "")
    {
      cout << "Naive stereo technique mean error = " << (naive_disp-gt).sqr().sum()/gt.height()/gt.width() << endl;
      cout << "MRF stereo technique mean error = " << (mrf_disp-gt).sqr().sum()/gt.height()/gt.width() << endl;

    }

  return 0;
}
