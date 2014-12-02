#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#include <cassert>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <string.h>
#include <utility>
#include <string>
using namespace std;

typedef unsigned int uint;
const int NUM_UNIFORM_CODES = 58;
const int NUM_LTP_CODES = NUM_UNIFORM_CODES + 1;

typedef vector<vector<double>> lbp_distance_map;
class PGMImage {
  public:
    // Constructors
    PGMImage() : width_(0), height_(0) { }
    PGMImage(const char* filename);
    
    // File I/O 
    void load(const string& filename);
    void save(char* filename);

    // Accessors
    int width() const
    { return width_; }

    int height() const
    { return height_; }

    int grey_lvl() const
    { return grey_lvl_; }

    void clear();

    // Check if loaded successfully from file
    operator bool() const
    { return (width_ > 0 && height_ > 0); }

    // Computer Visiony operations
    // Compute the average ltp distance between this image and other 
    // over the entirety of the image. 
    double average_ltp_distance(PGMImage& other) const;
    
    // Preprocessing routines. Each sets the ltps after completion
    void preprocess();
    void identity_preprocess(); // Do nothing 
    void gamma_correct();

    // Ready for use
    void calculate_ltp_distance_maps();

  private:
    static LBP_DICTIONARY /* TODO */

    // return the average of the lower and upper binary pattern distances
    // from point (x, y) in this image 
    // Simply performs a lookup in the pre-computed distances in ltp_distances
    double ltp_match_distance(int x, int y, pair<uint, uint> ltp) const;

    // These do the precomputation of ltp_distances, to be looked up at run time
    // by ltp_match_distance
    void calculate_ltp_match_distances();
    std::pair<double, double> calculate_ltp_match_distance(int x, int y, pair<uint, uint> ltp) const;

    // compute (and allocate if necessary) the ltps from the current data
    void set_ltps();

    // Both set to 0 if invalid
    int width_, height_, grey_lvl_;

    // Two dimensional array of width x height
    vector<vector<uint>> image_data;

    // LBPS themselves
    vector<vector<uint>> upper_lbps;
    vector<vector<uint>> lower_lbps;

    // Array of LBP distance transforms, indexed via the LBP_DICTIONARY
    vector<lbp_distance_map> upper_lbp_distance_transforms;
    vector<lbp_distance_map> lower_lbp_distance_transforms;
};

struct ltp {
  ltp(uint 
  uint upper;
  uint lower;


}

#endif

