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
typedef unsigned int uint;
using namespace std;

const int NUM_UNIFORM_CODES = 58;
const int NUM_LTP_CODES = NUM_UNIFORM_CODES + 1;

typedef std::pair<double, double>** ltp_distance_array;

// returns a pointer to the beginning of an array of all uniform codes.
uint *all_uniform_codes();

// returns true if lbp is uniform (has 2 or less 0-1 transitions)
bool is_uniform(uint lbp);

class PGMImage {
  public:
    // Constructors
    PGMImage() : width_(0), height_(0), data(NULL), ltps(NULL) { }
    PGMImage(const char* filename);
    PGMImage(const PGMImage& rhs);
    PGMImage(int size_x, int size_y, vector<pair<int, int>> white_pixels);

    ~PGMImage();
    
    // File I/O 
    void load(const string& filename);
    void save(char* filename) const;

    // Store all data to file including LBP arrays
    // and precomputed LBP distances
    void pickle(const string& filename) const;

    // Load all data including LBP arrays and
    // precomputed LBP distances from file
    void cucumber(const string& filename);
    void cucumber(ifstream& in_file);

    // Accessors
    pair<uint, uint> operator()(int x, int y) const
    { assert(*this); return ltps[x][y]; }

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
    void identity_preprocess(); // Do nothing 
		void DoG(double sigma1, double sigma2);
		void gaussian(double sigma);

  private:
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
    int width_, height_;

    // Max level of grey. Typically 255
    int grey_lvl_;

    // Two dimensional array of width x height
    unsigned int **data;

    // Two dimensional array of LTPs
    pair<uint, uint> ** ltps;

    ltp_distance_array ltp_distances[NUM_LTP_CODES];

    // File I/O private helpers
    void write_ltp_data(ofstream& out_file) const;
    void write_ltp_distance_data(ofstream& out_file) const;

    void load_ltp_data(ifstream& data_file);
    void load_ltp_distance_data(ifstream& data_file);

    string filename_;
};

#endif

