// Skeleton code for B657 A4 Part 2.
// D. Crandall
//
//
// Disparity Map are being saved as Disparitymap.png
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <CImg.h>
#include <assert.h>
#include <limits>
#include<float.h>
#include<algorithm>
using namespace cimg_library;
using namespace std;

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
        for (int i = 0 ; i < 2 ; i++) {
            CImg<double> cost(width, height,1,1);
            costs.push_back(cost);
        }
    }
    double getCost(int label, int i, int j) {
        CImg<double> cost = costs[label];
        return cost(j,i,0,0);
    }
    void setCost(int label, int i, int j, double val) {
        CImg<double> *cost = &costs[label];
        (*cost)(j, i, 0, 0) = val;
    }
};


LabelCost *labelCost;

// Calculating cost
CImg<double> assign_label(const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg , double fmean[], double fvariance[])
{
    double fixed_cost=40000.00;
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
                //cout<<"Started finding cost for each pixel"<<endl;
                double first_term = (1/(sqrt((2*3.14)*(2*3.14)*(2*3.14))*fvariance[0]*fvariance[1]*fvariance[2]));
                double red_term= (-0.5 * (1/fvariance[0]*fvariance[0])) *((img(j,i,0,0)-fmean[0])*(img(j,i,0,0)-fmean[0]));
                double green_term= (-0.5 * (1/fvariance[1]*fvariance[1])) *((img(j,i,0,1)-fmean[1])*(img(j,i,0,1)-fmean[1]));
                double blue_term= (-0.5 * (1/fvariance[2]*fvariance[2])) *((img(j,i,0,2)-fmean[2])*(img(j,i,0,2)-fmean[2]));
                double gaussian_cost = -log(first_term) * -1* (red_term + green_term + blue_term);
                //cout <<-log(first_term) <<" "<<red_term <<" "<<green_term <<" "<<blue_term <<" "<< gaussian_cost<<  endl;
                cost(j,i,0,0) = fixed_cost < gaussian_cost ? fixed_cost:gaussian_cost;
                L(j,i,0,0) = fixed_cost < gaussian_cost ? 0:1;
                //cout << "assigning label cost" <<endl;
                labelCost->setCost(0, i, j, fixed_cost/8000);
                labelCost->setCost(1, i, j, gaussian_cost/8000);
            }
        }
    }
    cout << "Assign Label Completed" << endl;
    L.get_normalize(0,255).save("Disparitymap.png");
    return L;
}

double cost_function (const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg, int i, int j, int l){
    
    for(int k = 0;k < fg.size() ; k++)
    {
        if ( j == fg[k].col && i == fg[k].row ) {
            //cout<<"checking for foreground"<<endl;
            if (l == 1){
                //cout << "LABEL 1 - FOREGROUND PIXEL" << endl;
                return 0;}
            else
                return 1000000;
        }
        
    }
    
    for(int k = 0 ; k < bg.size() ; k++)
    {
        if ( j == bg[k].col && i == bg[k].row ) {
            //cout<<"checking for background"<<endl;
            if (l == 0){
                //cout << "LABEL 0 - BG PIXEL" << endl;
                return 0;}
            else
                return 1000000;
        }
        
    }
    
    if (l == 0){
        double value_0 = labelCost->getCost(0, i, j);
        //cout<<value_0<<"This is the value for label 0"<<endl;
        return value_0;}
    else{
        double value_1 = labelCost->getCost(1, i, j);
        //cout<<value_1<<"This is the value for label 1"<<endl;
        return value_1;}
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

CImg<double> naive_segment(const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg)
{
    double fmean[3]={0,0,0}, fvariance[3]={0,0,0}, bmean[3]={0,0,0}, bvariance[3]={0,0,0};
    // Calculate Mean for Foreground
    for(int i=0;i< fg.size();i++)
    {
        fmean[0]+=img(fg[i].col,fg[i].row,0,0);
        fmean[1]+=img(fg[i].col,fg[i].row,0,1);
        fmean[2]+=img(fg[i].col,fg[i].row,0,2);
    }
    fmean[0]=fmean[0]/fg.size();
    fmean[1]=fmean[1]/fg.size();
    fmean[2]=fmean[2]/fg.size();
    cout<< "Mean"<<endl;
    cout<<fmean[0] <<" "<< fmean[1] <<" "<< fmean[2] <<endl ;
    
    // Calculate Variance for Foreground
    for(int i=0;i< fg.size();i++)
    {
        fvariance[0]+= (fmean[0] - img(fg[i].col,fg[i].row,0,0)) * (fmean[0] - img(fg[i].col,fg[i].row,0,0));
        fvariance[1]+=(fmean[1]-img(fg[i].col,fg[i].row,0,1)) * (fmean[1]-img(fg[i].col,fg[i].row,0,1)) ;
        fvariance[2]+= (fmean[2] -img(fg[i].col,fg[i].row,0,2)) * (fmean[2] -img(fg[i].col,fg[i].row,0,2)) ;
    }
    fvariance[0]=fvariance[0]/fg.size();
    fvariance[1]=fvariance[1]/fg.size();
    fvariance[2]=fvariance[2]/fg.size();
    cout<< "Variance"<<endl;
    cout<<fvariance[0] <<" "<< fvariance[1] <<" "<< fvariance[2] <<endl ;
    
    CImg<double> result=assign_label(img,fg,bg,fmean,fvariance);
    cout<<"COMPLETED"<<endl;
    return result;
}

double smoothness_function(int l1, int l2){
    if (l1 == l2)
        return 0;
    else
        return 1;
}

double createEnergy(const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg, Pixel *pixel, int l1, int i, int j, int dir) {
    //double energy= DBL_MAX;
    double energy1 ;
    double energy2 ;
    double return_energy;
    
    for (int l2 = 0 ; l2 < 2 ; l2++) {
        double cost_value = cost_function(img, fg, bg, i, j, l2);
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
//cost_function(img, fg, bg, i, j, l2)

CImg<double> mrf_segment(const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg)
{
  // implement this in step 3...
  //  this placeholder just returns a random disparity map by calling naive_segment
    Pixels prev(img.height(), img.width());
    Pixels curr(img.height(), img.width());
    //CImg<double> cost0(img.width(), img.height());
    //CImg<double> cost1(img.width(), img.height());
    

    /*for (int i = 0 ; i < img.height() ; i++) {
        for (int j = 0 ; j < img.width() ; j++) {
            cost0(i,j) = 10;
            cost1(i,j) = 5;
            
        }
    }*/
    for (int itr = 1 ; itr <= 1 ; itr++) {
        
        for (int i = 0 ; i < img.height() ; i++) {
            //cout<<"One width done"<<endl;
            for (int j = 0 ; j < img.width() ; j++) {
                Pixel *currentPixel = curr.getPixel(i, j);

                for (int dir = 0 ; dir < 4 ; dir++) {
                    if (!curr.hasNext((Direction) dir))
                        continue;
                    
                    Pixel *nextPixel = curr.getNext((Direction) dir);

                    Pixel *currentPixelPreItr = prev.getPixel(i, j);

                    if (nextPixel != NULL) {
                        
                        for (int l = 0 ; l < 2 ; l++) {
                            double energy = createEnergy(img, fg, bg, currentPixelPreItr, l, i, j, dir);
                            
                            nextPixel->getLabel(l)->setMessage((Direction) ((dir + 2) % 4), energy);
                            
                            //cout<<"this is message" << nextPixel->getLabel(l)->getMessage((Direction) ((dir + 2) % 4)) << endl;
                        }
                    }
                }
            }
        }

        prev = curr;
    }

    CImg<double> result(img.width(), img.height());

    for (int i = 0 ; i < img.height() ; i++) {
        cout<<"Labelling has started"<<endl;
        for (int j = 0; j < img.width(); j++) {
            //int intLabel = 0;
            int label = 0;
            double temp_0;
            double temp_1;
            
            //double max = numeric_limits<double>::max();
            double sum = 10000000;

            for (int l = 0 ; l < 2 ; l++) {
                if (l==0){
                    temp_0 = cost_function(img, fg, bg, i, j, l) + curr.getPixel(i, j)->getLabel(l)->getBeliefSum();
                }
                if (l==1) {
                    temp_1 = cost_function(img, fg, bg, i, j, l) + curr.getPixel(i, j)->getLabel(l)->getBeliefSum();

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





// Take in an input image and a binary segmentation map. Use the segmentation map to split the 
//  input image into foreground and background portions, and then save each one as a separate image.
//
// You'll just need to modify this to additionally output a disparity map.
//
void output_segmentation(const CImg<double> &img, const CImg<double> &labels, const string &fname)
{
  // sanity checks. If one of these asserts fails, you've given this function invalid arguments!
  assert(img.height() == labels.height());
  assert(img.width() == labels.width());

  CImg<double> img_fg = img, img_bg = img;

  for(int i=0; i<labels.height(); i++)
    for(int j=0; j<labels.width(); j++)
      {
	if(labels(j,i) == 0)
	  img_fg(j,i,0,0) = img_fg(j,i,0,1) = img_fg(j,i,0,2) = 0;
	else if(labels(j,i) == 1)
	  img_bg(j,i,0,0) = img_bg(j,i,0,1) = img_bg(j,i,0,2) = 0;
	else
	  assert(0);
      }

  img_fg.get_normalize(0,255).save((fname + "_fg.png").c_str());
  img_bg.get_normalize(0,255).save((fname + "_bg.png").c_str());
}

int main(int argc, char *argv[])
{
  if(argc != 3)
    {
      cerr << "usage: " << argv[0] << " image_file seeds_file" << endl;
      return 1;
    }

  string input_filename1 = argv[1], input_filename2 = argv[2];

  // read in images and gt
  CImg<double> image_rgb(input_filename1.c_str());
  CImg<double> seeds_rgb(input_filename2.c_str());

  // figure out seed points 
  vector<Point> fg_pixels, bg_pixels;
  for(int i=0; i<seeds_rgb.height(); i++)
    for(int j=0; j<seeds_rgb.width(); j++)
      {
	// blue --> foreground
	if(max(seeds_rgb(j, i, 0, 0), seeds_rgb(j, i, 0, 1)) < 100 && seeds_rgb(j, i, 0, 2) > 100)
	  fg_pixels.push_back(Point(j, i));

	// red --> background
	if(max(seeds_rgb(j, i, 0, 2), seeds_rgb(j, i, 0, 1)) < 100 && seeds_rgb(j, i, 0, 0) > 100)
	  bg_pixels.push_back(Point(j, i));
      }

  // do naive segmentation
  CImg<double> labels = naive_segment(image_rgb, fg_pixels, bg_pixels);
  output_segmentation(image_rgb, labels, input_filename1 + "-naive_segment_result");
  //test
  // do mrf segmentation
  labels = mrf_segment(image_rgb, fg_pixels, bg_pixels);
//  output_segmentation(image_rgb, labels, input_filename1 + "-mrf_segment_result");

  return 0;
}
