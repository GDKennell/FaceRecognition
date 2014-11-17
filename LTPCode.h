#ifndef LTPCODE_H
#define LTPCODE_H
#include <map>

typedef unsigned int uint;

class LBPmap{
  LBPmap(uint LBPradius);
  uint lbp_encode(uint LBP[LBPradius]);
  private:
  const uint perimeter;
  map<uint, uint> lbp_code_table;
};

#endif
