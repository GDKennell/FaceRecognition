#include <iostream>
#include <fstream>

#include "PGMImage.h"

using namespace std;

PGMImage::PGMImage(char* filename) {
  load(filename);
}

PGMImage::PGMImage(PGMImage& rhs) {
  data = new unsigned int*[rhs.width_];
  for (int x = 0; x < rhs.width_; ++x) {
    data[x] = new unsigned int[rhs.height_];
  }

  for (int x = 0; x < rhs.width_; ++x) {
    for (int y = 0; y < rhs.height_; ++y) {
      data[x][y] = rhs.data[x][y];
    }
  }

  width_ = rhs.width_;
  height_ = rhs.height_;
  grey_lvl_ = rhs.grey_lvl_;
}

PGMImage::~PGMImage() {
  clear();
}

// File I/O 
void PGMImage::load(char* filename) {
  ifstream img_fs;
  img_fs.open(filename);
  

  if (!img_fs) {
    cerr<<"Couldn't open image \""<<filename<<"\""<<endl;
    return;
  }
  string first_ln, comment_ln;
  getline(img_fs, first_ln);
  getline(img_fs, comment_ln);

  img_fs >> width_ >> height_ >> grey_lvl_;
  clear();
  data = new unsigned int*[width_];
  for (int x = 0; x < width_; ++x) {
    data[x] = new unsigned int[height_];
  }

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      char ch;
      img_fs.read(&ch, 1);
      data[x][y] = 0;
      memcpy(&data[x][y], &ch, 1);
    }
  }
}

void PGMImage::save(char* filename) const {
  ofstream new_image;
  new_image.open(filename);
  if (!new_image) {
    cerr<<"Failed to open output file \""<<filename<<"\""<<endl;
    return;
  }

  new_image << "P5\n#Image saved initially as "<<filename<<".\n"<<width_<<' '<<height_<<endl<<grey_lvl_<<endl;
  
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      new_image.write((const char*)&data[x][y], 1);
    }
  }
  new_image.close();
}

void PGMImage::clear() {
  if (data) {
    for (int x = 0; x < width_; ++x) {
      delete[] data[x];
    }
    delete[] data;
  }
}

