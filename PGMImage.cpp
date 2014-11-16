#include <iostream>
#include <cmath>
#include <fstream>
#include "PGMImage.h"

#define UNIFORM -1
typedef uint uint;

using namespace std;

int my_abs(int x) { return (x < 0) ? -x : x; }

PGMImage::PGMImage(const char* filename) {
  data = NULL;
  ltps = 0;
  load(filename);
}

PGMImage::PGMImage(const PGMImage& rhs) {
  data = new uint*[rhs.width_];
  for (int x = 0; x < rhs.width_; ++x) {
    data[x] = new uint[rhs.height_];
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

// File I/O -- puts image into data
void PGMImage::load(const char* filename) {
  ifstream img_fs;
  img_fs.open(filename);

  if (!img_fs) {
    cerr<<"Couldn't open image \""<<filename<<"\""<<endl;
    return;
  }
  string first_ln, comment_ln;
  getline(img_fs, first_ln);
//  getline(img_fs, comment_ln);

  img_fs >> width_ >> height_ >> grey_lvl_;
  clear();
  data = new uint*[width_];
  for (int x = 0; x < width_; ++x) {
    data[x] = new uint[height_];
  }

  for (int x = 0; x < width_; ++x) {
    for (int y = 0; y < height_; ++y) {
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
  
  for (int x = 0; x < width_; ++x) {
    for (int y = 0; y < height_; ++y) {
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
  if(ltps) {
    for(int i = 0; i < width_; i++) {
      delete[] ltps[i];
    }
    delete[] ltps;
  }
}

// Preprocessing routines

void PGMImage::preprocess_identity(){
  set_ltps();
}


// tolerance within which a pixel is considered as 0
// rather than 1 for above or -1 for below
const int ltp_tolerance = 10;

const int num_neighbors = 8;

// set bit bit_i of number to value
// bit_i of 0 is least significant bit
void set_bit(uint& number, int bit_i, bool value) {
  uint val = value ? 1 : 0;
  val = val << bit_i;
  number |= val;
}

// return lbp_upper, lbp_upper as a pair of uints
pair<uint, uint> ltp_to_lbps(int ltp[num_neighbors]){
  uint lbp_lower, lbp_upper;
  lbp_lower = lbp_upper = 0;
  for(i = 0; i < num_neighbors; i++){
    if(ltp[i] == -1){
      set_bit(lower, i, 1);
      set_bit(upper, i, 0);
    }
    else if(ltp[i] == 1){
      set_bit(upper, i, 1);
      set_bit(lower, i, 0);
    }
    else{
      set_bit(lower, i, 0);
      set_bit(upper, i, 0);
    }
  }
  if(!is_uniform(lbp_lower))
    lbp_lower = UNIFORM;
  if(!is_uniform(lbp_upper))
    lbp_upper = UNIFORM;
  pair<uint, uint> lbps(lbp_upper, lbp_lower);
  return lbps;
}

// return 0, 1, or -1 depending on ltp_tolerance
int compare_cells(int center, int outer){
  if(my_abs(outer - center) < ltp_tolerance)
    return 0;
  else if(outer - center > 0)
    return 1;
  else 
    return -1;
}

// set or reset the lbps_upper and lbps_lower arrays
void PGMImage::set_ltps() {
  if(!ltps) {
    ltps = new pair<uint, uint> *[height_];
    for(int i = 0; i < height_; i++){
      ltps[i] = new unsigned int *[width_];
    }
  }
  uint ltp[num_neighbors];
  for(int i = 1; i < height_ - 1; i++){
    for(int j = 1; j < width_ - 1; j++){
      int center = data[i][j];
      vector<int, int> perimeter = manhattan_circle(i, j, 1);
      for(int i = 0; i < num_neighbors; i++){
        int x = perimeter[i].first;
        int y = perimeter[i].second;
        ltp[i] = compare_cells(center, data[x][y]);
      }
      ltps[i][j] = ltp_to_lbps(ltp);
    }
  }
}

bool is_uniform(uint lbp) {
  int num_transitions = 0;
  for (int i = 0; i < num_neighbors; ++i) {
    bool this_bit = ((lbp & (1 << i)) != 0);
    bool next_bit = ((lbp & (1 << ((i + 1) % num_neighbors))) != 0);

    if (this_bit != next_bit)
      ++num_transitions;
  }
  return (num_transitions <= 2);
}

vector<pair<int, int>> manhattan_circle(int center_x, int center_y, int radius){
  // This creates a vector of indices in the shape of a circle under the
  // Manhattan metric. EG:
  // [-1,1]   [0,1]   [1,1]
  // [-1,0]           [1,0]
  // [-1,-1]  [0,-1]  [1,-1]
  // unrolls to 
  // [[-1,1], [0,1], [1,1], [1,0], [1,-1], [0,-1], [-1,-1], [-1,0]]
  vector<pair<int, int>> perimeter;
  // Top row of the circle
  for(int i = -radius; i <= radius; i++){
    pair<int, int> square(center_x + i, center_y + radius);
    perimeter.push_back(square);
  }
  // Right descending column
  for(int i = radius - 1; i >= -radius; i--){
    pair<int, int> square(center_x + radius, center_y + i);
    perimeter.push_back(square);
  }
  // Bottom row backwards
  for(int i = radius - 1; i >= -radius; i--){
    pair<int, int> square(center_x + i, center_y - radius);
    perimeter.push_back(square);
  }
  // Left ascending column
  for(int i = -radius + 1; i <= radius - 1; i++){
    pair<int, int> square(center_x - radius, center_y + i);
    perimeter.push_back(square);
  }
  return perimeter;
}

double PGMImage::ltp_match_distance(int x, int y, pair<uint, uint> ltp) const {
  // Return the average of the distance between the upper and lower lbps

  // Using boxes of increasing size. Not quite optimal - should be 
  // a round search circle of given radius to minimzie euclidian distance
  // but this will do for now
  const int fallthrough_max_radius = 200; // NOTE THAT CURRENTLY, this is
                                          // TOTALLY ARBITRARY -- todo
  int upper_match_radius = -1;
  int lower_match_radius = -1;

  // Set max_radius for search
  int max_x = max(width_ - 1 - x, x);
  int max_y = max(height_ - 1 - y, y);
  int max_radius = max(max_x, max_y);

  // Begin the search
  for(int search_radius = 0; search_radius <= max_radius; i++){
    // This constitutes our search path
    auto search_list = manhattan_circle(x, y, search_radius);

    // Go along path of manhattan radius search_radius searching for the best match
    for(auto i = search_list.begin(); i != search_list.end(); i++){
      const uint test_lbp_upper = ltp.first;
      const uint test_lbp_lower = ltp.second;
      const int search_x = i->x;
      const int search_y = i->y;

      // First, check bounds. There are no lbps associated with the outermost rows
      // of the images
      if(search_x <= 0 || search_x >= width_ - 1)
        continue;
      if(search_y <= 0 || search_y >= height_ -1)
        continue;
      const uint lbp_upper = ltps[search_x][search_y].first;
      const uint lbp_lower = ltps[search_x][search_y].second;

      // Now check for matches
      if(test_lbp_upper == lbp_upper){
        if(upper_match_radius != -1){
          double dist = sqrt(pow(search_x - x, 2) + pow(search_y - y, 2));
          upper_match_radius = min(upper_match_radius, dist);
        }
      }
      if(test_lbp_lower == lbp_lower){
        if(lower_match_radius != -1){
          double dist = sqrt(pow(search_x - x, 2) + pow(search_y - y, 2));
          upper_match_radius = min(upper_match_radius, dist);
        }
      }
    }
    // Once we've found matches, circumscribe our manhattan circle with a regular
    // circle, and then search for new matches in that radius's manhattan circle.
    if(upper_match_radius != -1 && lower_match_radius != -1){
      max_radius = ceil(max(upper_match_radius, lower_match_radius));
    }
  }
  if(upper_match_radius == -1)
    upper_match_radius = fallthrough_max_radius;
  if(lower_match_radius == -1)
    lower_match_radius = fallthrough_max_radius;
  
  return (upper_match_radius + lower_match_radius) / 2;
}

double PGMImage::average_ltp_distance(PGMImage& other) const {
  double total_distance = 0;
  for(int i = 1; i < width_ - 1; i++)
    for(int j = 1; j < height_ - 1; j++)
      total_distance += lbp_match_distance(i, j, other.ltps[i][j]);

  return total_distance /= ((width_ - 2)*(height_ - 2));
}
