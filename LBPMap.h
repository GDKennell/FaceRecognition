#ifndef LTPCODE_H
#define LTPCODE_H
#include <map>

typedef unsigned int uint;

const uint LBPradius = 1;
const uint perimeter = 8 * LBPradius;

extern const uint NON_UNIFORM_CODE;

uint rotate_bits(uint lbp, uint diameter);

class LBPMap{
  public:
    static LBPMap& shared_map();
    uint lbp_encode(uint LBP[LBPradius]);
    uint lbp_encode(uint LBP);

  private:
    LBPMap();
    std::map<int, uint> lbp_code_table;
};

#endif
