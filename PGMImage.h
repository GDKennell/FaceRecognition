#ifndef PGMIMAGE_H
    unsigned int **lpbs_lower;
#define PGMIMAGE_H

#include <cassert>
#include <utility>

class PGMImage {
  public:
    // Constructors
    PGMImage() : width_(0), height_(0), data(NULL) { }
    PGMImage(const char* filename);
    PGMImage(const PGMImage& rhs);

    ~PGMImage();
    
    // File I/O 
    void load(const char* filename);
    void save(char* filename) const;

    // Accessors
    pair<uint, uint> operator()(int x, int y) const
    { assert(*this); return lbps[x][y]; }

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

  private:
    // return the average of the lower and upper binary pattern distances
    // from point (x, y) in this image 
    double lbp_match_distance(int x, int y, pair<uint, uint> ltp) const;
    
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
};

#endif

