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

  private:
    // Both set to 0 if invalid
    int width_, height_;

    // Max level of grey. Typically 255
    int grey_lvl_;

    // Two dimensional array of width x height
    unsigned int **data;
};

#endif

