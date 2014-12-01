#ifndef LTPCODE_H
#define LTPCODE_H
#include <map>

typedef unsigned int uint;

const uint LBPradius = 1;
const uint perimeter = 8 * LBPradius;

extern const uint NON_UNIFORM_CODE;
const uint MAX_CODE = 0xff;

uint rotate_bits(uint lbp, uint diameter);

class LBPMap{
  public:
    // Singleton accessor
    static LBPMap& shared_map();

    uint lbp_encode(uint LBP[LBPradius]);
    uint lbp_encode(uint LBP);

    uint lbp_decode(uint mapped_val);

  private:
    LBPMap();
    // Assuming we're using 0x100 as the signal value for
    // non-uniform codes
    // Storing -1 as signal value for all unused (non-uniform) codes
    int lbp_code_table[MAX_CODE + 2];
};

#endif
