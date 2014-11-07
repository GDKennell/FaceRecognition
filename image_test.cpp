#include <iostream>

#include "PGMImage.h"

using namespace std;

float abs(float x) {
  if (x < 0) 
    return -x;
  return x;
}

float gradient(const PGMImage& img, int x, int y);

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr<<"pass in image file name / path as program argument"<<endl;
    return 1;
  }
  PGMImage img1(argv[1]);
  if (!img1) {
    cerr<<"Failed to load image from file \""<<argv[1]<<"\""<<endl;
    return 1;
  }

  PGMImage img2(img1);
  for (int x = 0; x < img1.width(); ++x) {
    for (int y = 0; y < img1.height(); ++y) {
      float edginess = gradient(img1, x, y);
      unsigned int val = edginess * img2.grey_lvl();
      if(!(val <= 255)) {
        cerr<<"Bad val computed, val("<<val<<") = edginess("<<edginess<<") * grey_lvl("<<img2.grey_lvl()<<")"<<endl;
        return 1;
      }
      img2(x,y) = edginess * img2.grey_lvl();
    }
  }

  img2.save("my_img.pgm");
  return 0;
}

float gradient(const PGMImage& img, int x, int y) {
  int sample_dist = 5;
  if (x - sample_dist < 0 || x + sample_dist >= img.width()
      || y - sample_dist < 0 || y + sample_dist >= img.height())
    return 0.0f;
  float avg_above_x = 0.0, avg_below_x = 0.0;
  for (int xi = x - sample_dist; xi < x; ++xi) {
    avg_above_x += img(xi, y) / (float)sample_dist;
  }
  for (int xi = x + 1; xi < x + sample_dist; ++xi) {
    avg_below_x += img(xi, y) / (float)sample_dist;
  }
  // change in x from 0.0 to 1.0
  // 1.0 being going from black to white, 0.0 being no color change
  float dx = abs(avg_above_x - avg_below_x) / (float)img.grey_lvl();

  float avg_above_y = 0.0, avg_below_y = 0.0;
  for (int yi = y - sample_dist; yi < y; ++yi) {
    avg_above_y += img(x, yi) / (float)sample_dist;
  }
  for (int yi = y + 1; yi < y + sample_dist; ++yi) {
    avg_below_y += img(x, yi) / (float)sample_dist;
  }
  // change in x from 0.0 to 1.0
  // 1.0 being going from black to white, 0.0 being no color change
  float dy = abs(avg_above_y - avg_below_y) / (float)img.grey_lvl();

  assert (dx >= 0 && dx <= 1.0f && dy >= 0 && dy <= 1.0f);
  return (dx + dy) / 2.0f;
}



