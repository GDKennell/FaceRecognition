#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#include <cassert>

class PGMImage {
  public:
    // Constructors
    PGMImage() : width_(0), height_(0), data(NULL) { }
    PGMImage(char* filename);
    PGMImage(PGMImage& rhs);

    ~PGMImage();
    
    // File I/O 
    void load(char* filename);
    void save(char* filename) const;

    // Accessors
    unsigned int& operator()(int x, int y) const
    { assert(*this); return data[x][y]; }

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
    // returns two PGMImages of the same dimensions
    // where each pixel is now the Local Binary Pattern
    // of the original pixel.
    pair<PGMImage, PGMImage> lbps() const;

    // Returns the nearest lbp code to (x,y) that matches lbp
    // If lbp is non-uniform (more than two 0-1 transistions)
    // finds the nearest non-uniform code
    // If no match found, returns a point at distance width_
    pair<int, int> nearest_lbp_match(int x, int y, unsigned int lbp);

  private:
    // Both set to 0 if invalid
    int width_, height_;

    // Max level of grey. Typically 255
    int grey_lvl_;

    // Two dimensional array of width x height
    unsigned int **data;
};

#endif

