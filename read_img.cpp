#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;


int main(int argc, char** argv) {

  if (argc != 2) {
    cerr<<"Provide image file name / path as program argument"<<endl;
    return 1;
  }

  ifstream img_fs;
  img_fs.open(argv[1]);

  if (!img_fs) {
    cerr<<"Couldn't open image \""<<argv[1]<<"\""<<endl;
    return 1;
  }

  string first_ln, comment_ln;
  int width, height, grey_lvl;
  getline(img_fs, first_ln);
  getline(img_fs, comment_ln);
  
  img_fs >> width >> height >> grey_lvl;

  char **image_data = new char*[width];
  for (int i = 0; i < width; ++i) {
    image_data[i] = new char[height];
  }

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      img_fs.read(&image_data[x][y], 1);
      unsigned int int_val = 0;
      memcpy(&int_val, &image_data[x][y], 1);
      assert(image_data[x][y] == (char)int_val);
      if (!img_fs) {
        cerr<<"image file stream broke at x("<<x<<") y("<<y<<")"<<endl;
        return 1;
      }
    }
  }
  ofstream new_image;
  new_image.open("my_image.pgm");

  //blur
  char **new_image_data = new char*[width];
  for (int i = 0; i < width; ++i) {
    new_image_data[i] = new char[height];
  }
  int blur_radius = 10;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      unsigned int total_grey = 0;
      unsigned int num_samples = 0;
      for (int yi = y - blur_radius; yi < y + blur_radius; ++yi) {
        for (int xi = x - blur_radius; xi < x + blur_radius; ++xi) {
          if (yi >= 0 && yi < height && xi >= 0 && xi < width) {
            ++num_samples;
            unsigned int grey_val = 0;
            memcpy(&grey_val, &image_data[xi][yi], 1);
            total_grey += grey_val;
          }
        }
      }
      assert(num_samples > 0);
      unsigned int new_val = total_grey / num_samples;
      if(new_val > 255) {
        cerr<<"new_val is wonky, value is "<< new_val<<endl;
        cerr<<"failed at x("<<x<<"), y("<<y<<")"<<endl;
        return 1;
      }
      memcpy(&new_image_data[x][y], &new_val, 1);
    }
  }


  new_image << "P5\n#This should be a blur of the image \""<<argv[1]<<"\".\n"<<width<<' '<<height<<endl<<grey_lvl<<endl;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      new_image.write(&new_image_data[x][y], 1);
    }
  }
  new_image.close();
  img_fs.close();

  return 0;

}
