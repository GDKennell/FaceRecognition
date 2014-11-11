#include <iostream>
#include <fstream>

#include "PGMImage.h"

using namespace std;

int my_abs(int x) { return (x < 0) ? -x : x; }

PGMImage::PGMImage(const char* filename) {
  data = NULL;
  lpbs_upper = 0;
  lpbs_lower = 0;
  load(filename);
}

PGMImage::PGMImage(const PGMImage& rhs) {
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
  data = new unsigned int*[height_];
  for (int x = 0; x < height_; ++x) {
    data[x] = new unsigned int[width_];
  }

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      char ch;
      img_fs.read(&ch, 1);
      data[y][x] = 0;
      memcpy(&data[y][x], &ch, 1);
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
      new_image.write((const char*)&data[y][x], 1);
    }
  }
  new_image.close();
}

void PGMImage::clear() {
  if (data) {
    for (int x = 0; x < height_; ++x) {
      delete[] data[x];
    }
    delete[] data;
  }
}

// tolerance within which a pixel is considered as 0
// rather than 1 for above or -1 for below
const int ltp_tolerance = 10;

const int num_neighbors = 8;

// set bit bit_i of number to value
// bit_i of 0 is least significant bit
void set_bit(unsigned int& number, int bit_i, bool value) {
  unsigned int val = value ? 1 : 0;
  val = val << bit_i;
  number |= val;
}

// return lbp_lower, lbp_upper as unsigned ints
pair<unsigned int, unsigned int> get_code(unsigned int values[7]){
  
}

int compare_cells(int center, int outer){
  if(my_abs(outer - center) < ltp_tolerance)
    return 0;
  else if(outer - center > 0)
    return 1;
  else 
    return -1;
}

void PGMImage::set_lbps() {
  if(!(lbps_upper && lbps_lower)) {
    lbps_upper = new unsigned int *[height_];
    lbps_lower = new unsigned int *[height_];
    for(int i = 0; i < height_; i++){
      lbps_upper[i] = new unsigned int *[width_];
      lbps_lower[i] = new unsigned int *[width_];
    }
  }
  unsigned int code[7];
  for(int i = 1; i < height_ - 1; i++){
    for(int j = 1; j < width_ - 1; j++){
      //  0 1 2
      //  7   3
      //  6 5 4
      int center = data[i][j];
      code[0] = compare_cells(center, data[i-1][j-1]);
      code[1] = compare_cells(center, data[i-1][j]);
      code[2] = compare_cells(center, data[i-1][j+1]);
      code[3] = compare_cells(center, data[i][j+1]);
      code[4] = compare_cells(center, data[i+1][j+1]);
      code[5] = compare_cells(center, data[i+1][j]);
      code[6] = compare_cells(center, data[i+1][j-1]);
      code[7] = compare_cells(center, data[i][j-1]);
      pair<unsigned int, unsigned int> lbp = get_code(code);
    
    }
  }
}

pair<PGMImage, PGMImage> PGMImage::lbps() const {
  // For a given ternary code [-1, 0, 1]
  // LBP1 will map [-1, 0] -> [0], [1] -> [1]
  // LBP2 will map [-1] -> [1], [0, 1] -> [0]

  PGMImage lbp1(*this), lbp2(*this);

  for (int x = 1; x < width_ - 1; ++x) {
    for (int y = 1; y < height_ - 1; ++y) {
      lbp1(x,y) = 0;
      lbp2(x,y) = 0;

      int offsets[num_neighbors][2] = { {-1, -1},
                                        {0, -1},
                                        {1, -1},
                                        {1, 0},
                                        {1, 1},
                                        {0, 1},
                                        {-1, 1},
                                        {-1, 0} };

      unsigned int this_pixel = data[x][y];
      for (int i = 0; i < num_neighbors; ++i) {
        unsigned int neighbor_pixel = data[x+offsets[i][0]][y+offsets[i][1]];
        // Within tolerance, both get 0
        if (my_abs(neighbor_pixel - this_pixel) <= ltp_tolerance) {
          set_bit(lbp1(x,y), i, false);
          set_bit(lbp2(x,y), i, false);
        }
        // neighbor > this: 1 for lbp1, 0 for lbp2
        else if (neighbor_pixel > this_pixel) {
          set_bit(lbp1(x,y), i, true);
          set_bit(lbp2(x,y), i, false);
        }
        // neighbor < this: 0 for lbp1, 1 for lbp2
        else {
          set_bit(lbp1(x,y), i, false);
          set_bit(lbp2(x,y), i, true);
        }
      }
    }
  }

  return pair<PGMImage, PGMImage>(lbp1, lbp2);
}

bool is_uniform(unsigned int lbp) {
  int num_transitions = 0;
  for (int i = 0; i < num_neighbors; ++i) {
    bool this_bit = ((lbp & (1 << i)) != 0);
    bool next_bit = ((lbp & (1 << ((i + 1) % num_neighbors))) != 0);

    if (this_bit != next_bit)
      ++num_transitions;
  }
  return (num_transitions <= 2);
}

pair<int, int> PGMImage::nearest_lbp_match(int x, int y, unsigned int lbp) const {
  // Using boxes of increasing size. Not quite optimal - should be 
  // a round search circle of given radius to minimzie euclidian distance
  // but this will do for now

  int search_box_radius = 1;
  bool uniform = is_uniform(lbp);

  if (data[x][y] == lbp || (!uniform && !is_uniform(data[x][y])))
    return pair<int, int>(x,y);

  bool still_have_options = true;
  while(still_have_options) {
    still_have_options = false;
    // Top and bottom of search box
    if (search_box_radius >= 3 * height_) {
      cout<<"This is my conditional breakpoint lol"<<endl;
    }
    int top_y = y - search_box_radius, bottom_y = y + search_box_radius;
    if(uniform) {
    for (int xi = x - search_box_radius; xi <= x + search_box_radius; ++xi) {
      if (xi < 1 || xi >= width_ - 1)
        continue;
      
      if (top_y >= 1) { 
        still_have_options = true;
        if (data[xi][top_y] == lbp)
          return pair<int, int>(xi, top_y);
      }
      if (bottom_y < height_ - 1) {
        still_have_options = true;
        if (data[xi][bottom_y] == lbp)
          return pair<int, int>(xi, bottom_y);
      }
    }
    }
    else {
    for (int xi = x - search_box_radius; xi <= x + search_box_radius; ++xi) {
      if (xi < 1 || xi >= width_ - 1)
        continue;
      
      if (top_y >= 1) { 
        still_have_options = true;
        if (!is_uniform(data[xi][top_y]))
          return pair<int, int>(xi, top_y);
      }
      if (bottom_y < height_ - 1) {
        still_have_options = true;
        if (!is_uniform(data[xi][bottom_y]))
          return pair<int, int>(xi, bottom_y);
      }
    }

    }
    // Left and right edges of search box
    int left_x = x - search_box_radius, right_x = x + search_box_radius;
    if (uniform) {
    for (int yi = y - search_box_radius; yi <= y + search_box_radius; ++yi) {
      if (yi < 1 || yi >= height_ - 1)
        continue;

      if (left_x >= 1) {
        still_have_options = true;
        if (data[left_x][yi] == lbp)
          return pair<int, int>(left_x, yi);
      }
      if (right_x < width_ - 1) {
        still_have_options = true;
        if (data[right_x][yi] == lbp)
          return pair<int, int>(right_x, yi);
      }
    }
    }
    else {
   for (int yi = y - search_box_radius; yi <= y + search_box_radius; ++yi) {
      if (yi < 1 || yi >= height_ - 1)
        continue;

      if (left_x >= 1) {
        still_have_options = true;
        if (!is_uniform(data[left_x][yi]))
          return pair<int, int>(left_x, yi);
      }
      if (right_x < width_ - 1) {
        still_have_options = true;
        if (!is_uniform(data[right_x][yi]))
          return pair<int, int>(right_x, yi);
      }
    }

    }
    ++search_box_radius;
  }
  return pair<int, int>(x+width_, y);
}

