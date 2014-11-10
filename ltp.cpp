#include <iostream>
#include <math.h>

#include "PGMImage.h"

using namespace std;

float identity(float x) {
  return x;
}

float (*gain_fn)(float) = &identity;

// Finds average euclidian distance between a given pattern in lbp1
// and the nearest matching pattern in lbp2
float total_lbp_distance(const PGMImage& lbp1, const PGMImage& lbp2);

// Input two image file paths, output similarity as total distance and average distance
pair<float, float> ltp_comparison(const char *filename1, const char *filename2) {
  //calculate LTP's of each pixel in each image
  PGMImage img1(filename1), img2(filename2);
//  cout<<"Images loaded"<<endl;

  assert(img1 && img2);
  if (!(img1.width() == img2.width() && img1.height() == img2.height())) {
    cout<<"can't compare images of diffferent sizes. given \""<<filename1<<"\" of dimensions "<<img1.width()<<"x"<<img1.height();
    cout<<" and \""<<filename2<<"\" of dimensions "<<img2.width()<<"x"<<img2.height()<<endl;
    exit(1);
  }

 // cout<<"Calculating LBPs of image 1"<<endl;
  pair<PGMImage, PGMImage> img1_lbps = img1.lbps();
//  cout<<"Calculating LBPs of image 2"<<endl;
  pair<PGMImage, PGMImage> img2_lbps = img2.lbps();

//  cout<<"Calculating total distances for first LBP"<<endl;
  float total_dist_lbp1 = total_lbp_distance(img1_lbps.first, img2_lbps.first);
 // cout<<"Calculating total distances for second LBP"<<endl;
  float total_dist_lbp2 = total_lbp_distance(img1_lbps.second, img2_lbps.second);

  //optimal avg distance (given same image twice) == 0
  //pathological worst case average distance is approximately avg(width,height)/2

  // being generous with worst case for now, will need to tweak it,
  // especially once we start doing more interesting things than just
  // euclidian distance

//  cout<<"\n\nSum of total distances between the two images: "<<total_dist_lbp1 + total_dist_lbp2<<endl;
//  cout<<"average distance = "<<(total_dist_lbp1 + total_dist_lbp2)/((float)(2 * img1.width() * img2.height()))<<endl<<endl;
  float total_dist = total_dist_lbp1 + total_dist_lbp2;
  float avg_dist = (total_dist_lbp1 + total_dist_lbp2)/((float)(2 * img1.width() * img2.height()));
  return pair<float, float>(total_dist, avg_dist);
}

float total_lbp_distance(const PGMImage& lbp1, const PGMImage& lbp2) {
  //for each pixel in img1, search for matching code in img2
  //record distance between pixel in img1 and pixel w/ matching code in img2

  float total_distance = 0.0f;
  int num_points = 0;
  for (int x = 0; x < lbp1.width(); ++x) {
    for (int y = 0; y < lbp1.height(); ++y) {
      pair<int, int> match_loc = lbp2.nearest_lbp_match(x, y, lbp1(x,y));
      float dist = sqrt(pow(x - match_loc.first, 2) + pow(y - match_loc.second, 2));
      total_distance += gain_fn(dist);
      ++num_points;
    }
  }

  return total_distance;
}



