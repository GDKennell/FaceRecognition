#include <iostream>
#include <math.h>

#include "PGMImage.h"

typedef unsigned int uint;
using namespace std;

float identity(float x) {
  return x;
}

float (*gain_fn)(float) = &identity;

// Finds nearest match of lbp in lbp_arr
pair<int, int> nearest_lbp_match(int x, int y, unsigned int lbp, unsigned int** lbp_arr, int width, int height) const {

// Finds average euclidian distance between a given pattern in lbp1
// and the nearest matching pattern in lbp2
float total_lbp_distance(unsigned int** lbp1, unsigned int** lbp2, int width, int height);

// Input two image file paths, output similarity as total distance and average distance
pair<float, float> ltp_comparison(const PGMImage& img1, const PGMImage& img2) {
  //calculate LTP's of each pixel in each image
//  cout<<"Images loaded"<<endl;
  assert(img1 && img2);
  if (!(img1.width() == img2.width() && img1.height() == img2.height())) {
    cout<<"can't compare images of diffferent sizes. given \""<<filename1<<"\" of dimensions "<<img1.width()<<"x"<<img1.height();
    cout<<" and \""<<filename2<<"\" of dimensions "<<img2.width()<<"x"<<img2.height()<<endl;
    exit(1);
  }

 // cout<<"Calculating LBPs of image 1"<<endl;
  pair<unsigned int**, unsigned int**> img1_lbps = img1.lbps();
//  cout<<"Calculating LBPs of image 2"<<endl;
  pair<unsigned int**, unsigned int**> img2_lbps = img2.lbps();

//  cout<<"Calculating total distances for first LBP"<<endl;
  float total_dist_lbp1 = total_lbp_distance(img1_lbps.first, img2_lbps.first, img1.width(), img1.height());
 // cout<<"Calculating total distances for second LBP"<<endl;
  float total_dist_lbp2 = total_lbp_distance(img1_lbps.second, img2_lbps.second, img1.width(), img1.height());

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

float total_lbp_distance(unsigned int** lbp1, unsigned int** lbp2, int width, int height) {
  //for each pixel in img1, search for matching code in img2
  //record distance between pixel in img1 and pixel w/ matching code in img2

  float total_distance = 0.0f;
  int num_points = 0;
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      pair<int, int> match_loc = nearest_lbp_match(x, y, lbp1[x][y], lbp2, width, height);
      float dist = sqrt(pow(x - match_loc.first, 2) + pow(y - match_loc.second, 2));
      total_distance += gain_fn(dist);
      ++num_points;
    }
  }

  return total_distance;
}

pair<int, int> nearest_lbp_match(int x, int y, unsigned int lbp, unsigned int** lbp_arr, int width, int height) const {
  // Using boxes of increasing size. Not quite optimal - should be 
  // a round search circle of given radius to minimzie euclidian distance
  // but this will do for now

  int search_box_radius = 1;
  bool uniform = is_uniform(lbp);

  if (lbp_arr[x][y] == lbp || (!uniform && !is_uniform(lbp_arr[x][y])))
    return pair<int, int>(x,y);

  bool still_have_options = true;
  while(still_have_options) {
    still_have_options = false;
    // Top and bottom of search box
    if (search_box_radius >= 3 * height) {
      cout<<"This is my conditional breakpoint lol"<<endl;
    }
    int top_y = y - search_box_radius, bottom_y = y + search_box_radius;
    if(uniform) {
      for (int xi = x - search_box_radius; xi <= x + search_box_radius; ++xi) {
        if (xi < 1 || xi >= width - 1)
          continue;
        
        if (top_y >= 1) { 
          still_have_options = true;
          if (lbp_arr[xi][top_y] == lbp)
            return pair<int, int>(xi, top_y);
        }
        if (bottom_y < height - 1) {
          still_have_options = true;
          if (lbp_arr[xi][bottom_y] == lbp)
            return pair<int, int>(xi, bottom_y);
        }
      }
    }
    else {
      for (int xi = x - search_box_radius; xi <= x + search_box_radius; ++xi) {
        if (xi < 1 || xi >= width - 1)
          continue;
        
        if (top_y >= 1) { 
          still_have_options = true;
          if (!is_uniform(lbp_arr[xi][top_y]))
            return pair<int, int>(xi, top_y);
        }
        if (bottom_y < height - 1) {
          still_have_options = true;
          if (!is_uniform(lbp_arr[xi][bottom_y]))
            return pair<int, int>(xi, bottom_y);
        }
      }
    }
    // Left and right edges of search box
    int left_x = x - search_box_radius, right_x = x + search_box_radius;
    if (uniform) {
      for (int yi = y - search_box_radius; yi <= y + search_box_radius; ++yi) {
        if (yi < 1 || yi >= height - 1)
          continue;

        if (left_x >= 1) {
          still_have_options = true;
          if (lbp_arr[left_x][yi] == lbp)
            return pair<int, int>(left_x, yi);
        }
        if (right_x < width - 1) {
          still_have_options = true;
          if (lbp_arr[right_x][yi] == lbp)
            return pair<int, int>(right_x, yi);
        }
      }
    }
    else {
      for (int yi = y - search_box_radius; yi <= y + search_box_radius; ++yi) {
        if (yi < 1 || yi >= height - 1)
          continue;

        if (left_x >= 1) {
          still_have_options = true;
          if (!is_uniform(lbp_arr[left_x][yi]))
            return pair<int, int>(left_x, yi);
        }
        if (right_x < width - 1) {
          still_have_options = true;
          if (!is_uniform(lbp_arr[right_x][yi]))
            return pair<int, int>(right_x, yi);
        }
      }
    }
    ++search_box_radius;
  }
  return pair<int, int>(x+width, y);
}



