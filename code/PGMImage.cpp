#include "LBPMap.h"
#include "PGMImage.h"
#include <cstdlib>
#include <deque>
#include <cmath>


typedef unsigned int uint;

using namespace std;

vector<vector<double> > gaussWindow(double sigma, int filterRadius);
double ** gaussianFilter(uint ** image, 
                         uint height, uint width, 
                         double sigma, 
                         edge_behavior_t edge_behavior);
uint ** DoG(uint ** image, 
            uint height, uint width, 
            double sigma1, double sigma2,
            edge_behavior_t edge_behavior);
int roundOdd(double x);
int my_abs(int x) { return (x < 0) ? -x : x; }

const int fallthrough_max_radius = 6; // NOTE THAT CURRENTLY, this is
                                        // TOTALLY ARBITRARY -- todo
uint *all_uniform_codes() {
  const int num_uniform_codes = 58;
  static uint* the_codes = NULL;
  if (the_codes)
    return the_codes;
  uint count = 0;
  the_codes = new uint[num_uniform_codes];
  for (uint i = 0; i < 0x100; ++i) {
    if (is_uniform(i)) {
      the_codes[count++] = i;
    }
  }
  return the_codes;
}

PGMImage::PGMImage(const char* filename) {
  data = 0;
  ltps = 0;
  ltp_distances[0] = 0;
  load(filename);
}

PGMImage::PGMImage(int size_x, int size_y, vector<pair<int, int>> white_pixels){
  width_ = size_x;
  height_ = size_y;
  grey_lvl_ = 255;
  data = new uint*[width_];
  for (int x = 0; x < width_; ++x)
    data[x] = new uint[height_];

  for (int x = 0; x < width_; ++x)
    for (int y = 0; y < height_; ++y)
      data[x][y] = 255;

  for(auto it = white_pixels.begin(); it != white_pixels.end(); it++)
    data[it->first][it->second] = 0;
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
void PGMImage::load(const string& filename) {
  filename_ = filename;
  ifstream img_fs;
  //cout << "in PGMImage::load:\topening filename..." << endl;

  img_fs.open(filename.c_str());
  //cout << "in PGMImage::load:\topened..." << endl;

  if (!img_fs) {
    cerr<<"Couldn't open image \""<<filename<<"\""<<endl;
    exit(1);
  }
  string first_ln, comment_ln;
  getline(img_fs, first_ln);
  getline(img_fs, first_ln);

  img_fs >> width_ >> height_ >> grey_lvl_;
  clear();
  data = new uint*[width_];
  for (int x = 0; x < width_; ++x) {
    data[x] = new uint[height_];
  }

  //cout << "in PGMImage::load:\tdata arrays allocated" << endl;
  
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      char ch;
      img_fs.read(&ch, 1);
      data[x][y] = 0;
      memcpy(&data[x][y], &ch, 1);
    }
  }

  //cout << "in PGMImage::load:\tdone. Returning..." << endl;
}

void PGMImage::save(const char* filename) const {
  ofstream new_image;
  new_image.open(filename);
  if (!new_image) {
    cerr<<"Failed to open output file \""<<filename<<"\""<<endl;
    exit(1);
  }

  new_image << "P5\n#Image saved initially as "<<filename<<".\n"
						<<width_<<' '<<height_<<endl<<grey_lvl_<<endl;
  
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      new_image.write((const char*)&data[x][y], 1);
    }
  }
  new_image.close();
}

void PGMImage::pickle(const string& filename) const {
  assert(data && ltps && ltp_distances[0]);
  ofstream data_file;
  data_file.open(filename.c_str());
  if (!data_file) {
    cerr<<"Failed to open output file \""<<filename<<"\""<<endl;
    exit(1);
  }
  data_file<<width_<<'\n'<<height_<<'\n';
  data_file<<"#LTP data\n";
  write_ltp_data(data_file);
  data_file<<"#LTP distance data\n";
  write_ltp_distance_data(data_file);
  data_file.close();
}

void PGMImage::cucumber(const string& filename) {
  ifstream data_file;
  data_file.open(filename);
  if (!data_file) {
    cerr<<"Failed to open input file \""<<filename<<"\""<<endl;
    exit(1);
  }
  cucumber(data_file);
  data_file.close();
}
void PGMImage::cucumber(ifstream& data_file) {
  data_file >> width_ >> height_;
  load_ltp_data(data_file);
  load_ltp_distance_data(data_file);
}

void PGMImage::write_ltp_data(ofstream& out_file) const {
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      out_file<<ltps[x][y].first<<' '<<ltps[x][y].second<<' ';
    }
    out_file<<'\n';
  }
}

void PGMImage::write_ltp_distance_data(ofstream& out_file) const {
  for (int i = 0; i < NUM_LTP_CODES; ++i) {
    out_file<<"#Code "<<i<<" ("<<LBPMap::shared_map().lbp_decode(i)<<")\n";
    for (int y = 0; y < height_; ++y) {
      for (int x = 0; x < width_; ++x) {
        out_file<<ltp_distances[i][x][y].first<<' '<<ltp_distances[i][x][y].second<<' ';
      }
      out_file<<'\n';
    }
  } 
}
void PGMImage::load_ltp_data(ifstream& data_file) {
  //First line will be comment "#LTP data"
  string comment_line;
  getline(data_file, comment_line);

  if(!ltps) {
    //cout << "in PGMImage::set_ltps:\tallocating arrays" << endl;
    ltps = new pair<uint, uint> *[width_];
    for(int i = 0; i < width_; i++){
      ltps[i] = new pair<uint, uint> [height_];
    }
  }

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      data_file >> ltps[x][y].first >> ltps[x][y].second;
    }
  }
}

void PGMImage::load_ltp_distance_data(ifstream& data_file) {
  //First line will be comment "#LTP distance data
  string comment_line;
  getline(data_file, comment_line);

  for (int i = 0; i < NUM_LTP_CODES; ++i) {
    //Comment line before each line of form
    //#Code i (<lbp_code>)
    getline(data_file, comment_line);
    ltp_distances[i] = new pair<double, double>*[width_];
    for (int x = 0; x < width_; ++x) {
      ltp_distances[i][x] = new pair<double, double> [height_];
    }

    for (int y = 0; y < height_; ++y) {
      for (int x = 0; x < width_; ++x) {
        data_file >> ltp_distances[i][x][y].first >> ltp_distances[i][x][y].second;
      }
    }
  }

}

void PGMImage::clear() {
  if (data) {
    for (int x = 0; x < width_; ++x) {
      delete[] data[x];
      data[x] = 0;
    }
    delete[] data;
    data = 0;
  }
  if(ltps) {
    for(int i = 0; i < width_; i++) {
      delete[] ltps[i];
      ltps[i] = 0;
    }
    delete[] ltps;
    ltps = 0;
  }
}

// Preprocessing routines

void PGMImage::identity_preprocess(){
  //DoG(1,2, NONE);
  set_ltps();
  //save(filename_.c_str());
}
void PGMImage::DoG(double sigma1, double sigma2, edge_behavior_t edge_behavior){
	uint ** ptr_copy = data;
	data = ::DoG(data, height(), width(), sigma1, sigma2, edge_behavior);
	for(int i = 0; i < width(); i++)
		delete[] ptr_copy[i];
	delete[] ptr_copy;
}

void PGMImage::gaussian(double sigma, edge_behavior_t edge_behavior){
	double ** gauss = gaussianFilter(data, height(), width(), sigma, edge_behavior);
	for(int i = 0; i < width(); i++)
		for(int j = 0; j < height(); j++)
			data[i][j] = (uint)(round(gauss[i][j]));
}

void PGMImage::gamma_correct(double gamma){
  double ** data_copy = new double *[width_];
  double max_val = 0;
  for(int x = 0; x < width_; x++){
    data_copy[x] = new double[height_];
    for(int y = 0; y < height_; y++){
      data_copy[x][y] = pow(data[x][y], gamma);
      max_val = max(max_val, data_copy[x][y]);
    }
  }
  for(int x = 0; x < width_; x++)
    for(int y = 0; y < height_; y++)
      data[x][y] = (uint)(data_copy[x][y]*255/max_val);

  for(int x = 0; x < width_; x++)
    delete[] data_copy[x];
  delete[] data_copy;
}

void PGMImage::contrast_equalize(){
  double a = .1;
  double trim = 10;
  double sum = 0;
  for(int i = 0; i < width_; i++)
    for(int j = 0; j < height_; j++)
      sum += pow(data[i][j], a);
  sum /= width_*height_;
  sum = pow(sum, 1/a);
  for(int i = 0; i < width_; i++){
    for(int j = 0; j < height_; j++){
      double normed = ((double)data[i][j])/sum;
      data[i][j] = (uint)round(normed);
    }
  }
  sum = 0;
  for(int i = 0; i < width_; i++)
    for(int j = 0; j < height_; j++)
      sum += pow(min(trim, (double)data[i][j]), a);
  sum /= width_*height_;
  sum = pow(sum, 1/a);
  for(int i = 0; i < width_; i++){
    for(int j = 0; j < height_; j++){
      double normed = ((double)data[i][j])/sum;
      data[i][j] = (uint)round(normed);
    }
  }
  
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

// return lbp_upper, lbp_upper as a pair of uints
pair<uint, uint> ltp_to_lbps(int ltp[num_neighbors]){
  uint lbp_lower, lbp_upper;
  lbp_lower = lbp_upper = 0;
  for(int i = 0; i < num_neighbors; i++){
    if(ltp[i] == -1){
      set_bit(lbp_lower, i, 1);
      set_bit(lbp_upper, i, 0);
    }
    else if(ltp[i] == 1){
      set_bit(lbp_upper, i, 1);
      set_bit(lbp_lower, i, 0);
    }
    else{
      set_bit(lbp_lower, i, 0);
      set_bit(lbp_upper, i, 0);
    }
  }
  if(!is_uniform(lbp_lower))
    lbp_lower = NON_UNIFORM_CODE;
  if(!is_uniform(lbp_upper))
    lbp_upper = NON_UNIFORM_CODE;
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

vector<pair<int, int> >& manhattan_circle(int radius){
  static vector<pair<int, int> >cached_circles[2 * fallthrough_max_radius+1];
  if (!cached_circles[radius].empty()) {
    return cached_circles[radius];
  }

  // This creates a vector of indices in the shape of a circle under the
  // Manhattan metric. EG:
  // [-1,-1]   [0,-1]  [1,-1]
  // [-1, 0]           [1, 0]
  // [-1, 1]   [0, 1]  [1, 1]
  // unrolls to 
  // [[-1,-1], [0,-1], [1,-1], [1,0], [1,1], [0,1], [-1,1], [-1,0]]
  vector<pair<int, int>> perimeter;
  // Top row of the circle
  for(int i = -radius; i <= radius; ++i){
    pair<int, int> square(i, -radius);
    perimeter.push_back(square);
  }
  // Right descending column
  for(int i = -radius + 1; i <= radius; ++i){
    pair<int, int> square(radius, i);
    perimeter.push_back(square);
  }
  // Bottom row backwards
  for(int i = radius - 1; i >= -radius; --i){
    pair<int, int> square(i, radius);
    perimeter.push_back(square);
  }
  // Left ascending column
  for(int i = radius - 1; i >= -radius + 1; --i){
    pair<int, int> square(-radius, i);
    perimeter.push_back(square);
  }
  cached_circles[radius] = perimeter;
  return cached_circles[radius];
}

// set or reset the lbps_upper and lbps_lower arrays
void PGMImage::set_ltps() {
  if(!ltps) {
    //cout << "in PGMImage::set_ltps:\tallocating arrays" << endl;
    ltps = new pair<uint, uint> *[width_];
    for(int i = 0; i < width_; i++){
      ltps[i] = new pair<uint, uint> [height_];
    }
  }
  //cout << "in PGMImage::set_ltps:\tarrays allocated" << endl;
  int ltp[num_neighbors];
  for(int x = 1; x < width_- 1; x++){
    for(int y = 1; y < height_ - 1; y++){
      //cout << "in PGMImage::set_ltps:\taccessing data[" << x << "]["
           //<< y << "]" << endl;
      int center = data[x][y];
      vector<pair<int, int>> perimeter = manhattan_circle(1);
      //cout << "in PGMImage::set_ltps:\tgetting ltp at [" << x << ","
           //<< y << "]" << endl;
      for(int k = 0; k < num_neighbors; k++){
        int perimeter_x = x + perimeter[k].first;
        int perimeter_y = y + perimeter[k].second;
        //cout << "in PGMImage::set_ltps:\tchecking neighbor " << k << 
                //" = [" << x << "," << y << "] of width_ = " << 
                //width_ << " and height_ = " << height_ << endl;
        ltp[k] = compare_cells(center, data[perimeter_x][perimeter_y]);
      }
      //cout << "in PGMImage::set_ltps:\tdone." << endl;
      ltps[x][y] = ltp_to_lbps(ltp);
      //cout << "in PGMImage::set_ltps:\tsuper done." << endl;
    }
    //cout << "in PGMImage::set_ltps:\twalked a column" << endl;
  }
  //cout << "in PGMImage::set_ltps:\tDONE WITH PGMImage::set_ltps" << endl;

  // Precompute distances to all upper and lower ltp codes
  calculate_ltp_match_distances();
}

string pickle_data_filename(const string& filename) {
  string data_file_suffix = "_data.txt";
  string data_filename = filename.substr(0, filename.size() - 4);
  data_filename += data_file_suffix;
  return data_filename;
}

void PGMImage::calculate_ltp_match_distances() {
  ifstream pickle_data_file;
  pickle_data_file.open(pickle_data_filename(filename_));
  if (pickle_data_file) {
    cout<<"Found pickle data file \""<<pickle_data_filename(filename_)<<"\". Loading from it"<<endl;
    cucumber(pickle_data_file);
    pickle_data_file.close();
    return;
  }
  cout<<"No pickle data file found for image \""<<filename_<<"\". calcluating data"<<endl;
    
  uint *uniform_codes = all_uniform_codes();
  static int count = 1;
  cout<<"Precomputing ltp_match distances for image #"<<count++<<endl;
  //cout<<"allocating space for ltp distances"<<endl;
  for (int i = 0; i < NUM_LTP_CODES; ++i) {
    cout<<"Precomputing ltp_match distances for code #"<<i<<" of "<<NUM_UNIFORM_CODES + 1;
    cout<<" ("<<LBPMap::shared_map().lbp_decode(i)<<')'<<endl;
    double total_distance = 0.0;
    int count = 0;
    ltp_distances[i] = new pair<double, double>*[width_];
    //cout<<"width_: "<<width_<<", height_: "<<height_<<endl;
    for (int x = 0; x < width_; ++x) {
      ltp_distances[i][x] = new pair<double, double> [height_];
    }
    for (int x = 0; x < width_; ++x) {
//      cout<<"row x="<<x<<" of "<<width_<<endl;
      for (int y = 0; y < height_; ++y) {
        auto distances = calculate_ltp_match_distance(x,y, pair<uint, uint>(i, i));
        ltp_distances[i][x][y] = distances;
        total_distance += distances.first + distances.second;
        count += 2;
      }
    }
    cout<<"\tfinished, avg distance: "<<total_distance / ((double)count)<<endl<<endl;
  }
  pickle(pickle_data_filename(filename_));
}

pair<double, double> PGMImage::calculate_ltp_match_distance(int x, int y, pair<uint, uint> ltp) const {
  // Return the average of the distance between the upper and lower lbps

  // Using boxes of increasing size. Not quite optimal - should be 
  // a round search circle of given radius to minimzie euclidian distance
  // but this will do for now
  double upper_match_radius = -1;
  double lower_match_radius = -1;

  // Set max_radius for search
  int max_x = max(width_ - 1 - x, x);
  int max_y = max(height_ - 1 - y, y);
  int max_radius = 6;

  // Begin the search
  //cout << "in PGMImage::ltp_match_distance:\tsearching..." << endl;
  for(int search_radius = 0; search_radius <= max_radius; search_radius++){
    // This constitutes our search path
    auto search_list = manhattan_circle(search_radius);

    // Go along path of manhattan radius search_radius searching for the best match
    for(auto i = search_list.begin(); i != search_list.end(); i++){
      const uint test_lbp_upper = ltp.first;
      const uint test_lbp_lower = ltp.second;
      const int search_x = x + i->first;
      const int search_y = y + i->second;

      // First, check bounds. There are no lbps associated with the outermost rows
      // of the images
      if(search_x <= 0 || search_x >= width_ - 1)
        continue;
      if(search_y <= 0 || search_y >= height_ -1)
        continue;
      const uint lbp_upper = LBPMap::shared_map().lbp_encode(ltps[search_x][search_y].first);
      const uint lbp_lower = LBPMap::shared_map().lbp_encode(ltps[search_x][search_y].second);

      // Now check for matches
      if(test_lbp_upper == lbp_upper){
        double dist = sqrt(pow(search_x - x, 2) + pow(search_y - y, 2));
        if(upper_match_radius != -1){
          upper_match_radius = min((double)upper_match_radius, dist);
        }
        else{
          upper_match_radius = dist;
        }
      }
      if(test_lbp_lower == lbp_lower){
        double dist = sqrt(pow(search_x - x, 2) + pow(search_y - y, 2));
        if(lower_match_radius != -1){
          lower_match_radius = min((double)upper_match_radius, dist);
        }
        else{
          lower_match_radius = dist;
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

  return pair<double, double>(lower_match_radius, upper_match_radius);
}

double PGMImage::ltp_match_distance(int x, int y, pair<uint, uint> ltp) const {
  uint lower_code = LBPMap::shared_map().lbp_encode(ltp.first);
  uint upper_code = LBPMap::shared_map().lbp_encode(ltp.second);

  double lower_distance = ltp_distances[lower_code][x][y].first;
  double upper_distance = ltp_distances[upper_code][x][y].second;

  return (lower_distance + upper_distance) / 2.0;
}

double PGMImage::average_ltp_distance(const PGMImage& other) const {
  double total_distance = 0;
  for(int i = 1; i < width_ - 1; i++)
    for(int j = 1; j < height_ - 1; j++)
      total_distance += ltp_match_distance(i, j, other(i, j));

  return total_distance /= ((width_ - 2)*(height_ - 2));
}


uint ** DoG(uint ** image, 
            uint height, uint width, 
            double sigma1, double sigma2, 
            edge_behavior_t edge_behavior){
  uint ** newImage = new uint *[width];
  for(int i = 0; i < width; i++)
    newImage[i] = new uint[height];
  double ** img1 = gaussianFilter(image, height, width, sigma1, edge_behavior);
  double ** img2 = gaussianFilter(image, height, width, sigma2, edge_behavior);
  double maxVal = 0;
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      img1[i][j] = abs(img1[i][j] - img2[i][j]);
      maxVal = max(maxVal, img1[i][j]);
    }
  }
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      newImage[i][j] = (uint)(img1[i][j] * (255.0/maxVal));
    }
  }
  for(int i = 0; i < width; i++){
    delete[] img1[i];
    delete[] img2[i];
  }
  delete[] img1;
  delete[] img2;
  return newImage;
}

double ** gaussianFilter(uint ** image, 
                         uint height, uint width, 
                         double sigma,
                         edge_behavior_t edge_behavior){
	// create 2d array to store result
  double ** newImage = new double *[width];
  for(int i = 0; i < width; i++)
    newImage[i] = new double[height];
	// creates gaussian window
  int filterRadius = ceil(3*sigma); // capture 95% of the variation
  //cout << filterRadius << endl;
  vector<vector<double> > window = gaussWindow(sigma, filterRadius);
  //cout << window.size() << "x" << window[0].size() << endl;

  for(int c_x = 0; c_x < width; c_x++)
    for(int c_y = 0; c_y < height; c_y++)
    	newImage[c_x][c_y] = 0;

  for(int c_x = 0; c_x < width; c_x++){
    for(int c_y = 0; c_y < height; c_y++){
      // off_ = offset
      for(int off_x = -filterRadius; off_x <= filterRadius; off_x++){
        for(int off_y = -filterRadius; off_y <= filterRadius; off_y++){
          // filt_ = filter
					int filt_x = off_x + filterRadius;
					int filt_y = off_y + filterRadius;
          // img = image
          int img_x = c_x + off_x;
          int img_y = c_y + off_y;
		      // Do mirroring
          switch(edge_behavior){
            case MIRROR:
              if(img_y < 0) // reflect across 0 Y axis
                img_y = -img_y;
              else if(img_y > height - 1) // reflect across height - 1 Y axis
                img_y = (height - 1) - (img_y - (height - 1));
              if(img_x < 0) // reflect across 0 X axis
                img_x = -img_x;
              else if(img_x > width - 1) // reflect across height - 1 X axis
                img_x = (width - 1) - (img_x - (width - 1));
              break;
            case EDGE_ACCUMULATE:
					    if(img_x < 0)
					    	img_x = 0;
					    else if(img_x > width - 1)
					    	img_x = width - 1;
					    if(img_y < 0)
					    	img_y = 0;
					    else if(img_y > height - 1)
					    	img_y = height - 1;
              break;
            default:
					    if(img_x < 0)
                continue;
					    else if(img_x > width - 1)
                continue;
					    if(img_y < 0)
                continue;
					    else if(img_y > height - 1)
                continue;
          }
          newImage[img_x][img_y] += window[filt_x][filt_y]*image[c_x][c_y];
          if(newImage[img_x][img_y] > 255) newImage[img_x][img_y] = 255;
        }
      }
    }
  }
  return newImage;
}

int roundOdd(double x){
  x = round(x);
  int y = (int)x;
  while(!(y % 2))
    y++;
  return y;
}

vector<vector<double> > gaussWindow(double sigma, int filterRadius){
  vector<vector<double> > gaussXWindow(2*filterRadius + 1);
  vector<vector<double> > gaussYWindow(2*filterRadius + 1);
  vector<vector<double> > gaussXYWindow(2*filterRadius + 1);
  for(int i = -filterRadius; i <= filterRadius; i++){
    int y = i + filterRadius;
    gaussXWindow[y].resize(2*filterRadius + 1);
    gaussYWindow[y].resize(2*filterRadius + 1);
    gaussXYWindow[y].resize(2*filterRadius + 1);
  }
  for(int i = -filterRadius; i <= filterRadius; i++){
    for(int j = -filterRadius; j <= filterRadius; j++){
      int y = i + filterRadius, x = j + filterRadius;
      gaussXWindow[x][y] = i;
      gaussYWindow[y][x] = i;
    }
  }
  double gaussSum = 0;
  for(int i = -filterRadius; i <= filterRadius; i++){
    for(int j = -filterRadius; j <= filterRadius; j++){
      int y = i + filterRadius, x = j + filterRadius;
      gaussXYWindow[y][x] = exp(-pow(gaussXWindow[y][x],2)/(2*pow(sigma,2)) -
                                pow(gaussYWindow[y][x],2)/(2*pow(sigma,2)));
      gaussSum += gaussXYWindow[y][x];
    }
  }
  for(int i = -filterRadius; i <= filterRadius; i++){
    for(int j = -filterRadius; j <= filterRadius; j++){
      int y = i + filterRadius, x = j + filterRadius;
      gaussXYWindow[y][x] /= gaussSum;
    }
  }
  return gaussXYWindow;
}

