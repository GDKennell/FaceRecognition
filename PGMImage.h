#ifndef PGMIMAGE_H
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
    void set_ltps();
    double average_ltp_distance(PGMImage& other) const;

  private:
    // return the average of the lower and upper binary pattern distances
    // from point (x, y) in this image 
    double lbp_match_distance(int x, int y, pair<uint, uint> ltp) const;

    // Both set to 0 if invalid
    int width_, height_;

    // Max level of grey. Typically 255
    int grey_lvl_;

    // Two dimensional array of width x height
    unsigned int **data;

    // Two dimensional array of LPBs
    pair<uint, uint> ** ltps;
};

#endif

