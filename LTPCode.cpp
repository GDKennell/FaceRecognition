#include "LTPcode.h"
#include <stdexcept>
#include <map>
using namespace std;
typdef unsigned int uint;

LBPmap::LBPmap(uint LBPradius)
  : perimeter(8*LBPradius) {
  uint count = 0;
  lbp_code_table.insert(0, count++);
  for(uint i = 1; i < perimeter; i++){
    uint lbp = (uint)1 << i - 1;
    for(uint j = 0; j < perimeter){
      lbp_code_table.insert(lbp, count++);
      lbp = rotate_bits(lbp, perimeter);
    }
  }
  uint lbp = (uint)1 >> perimeter - 1;
  lbp_code_table.insert(lbp, count++);
  lbp_code_table.insert(-1, count++); // UNIFORM CODES
}

// Rotate bits one step to the left, with a given diameter. EG
// rotate_bits(001001, 4) returns 000011.
uint rotate_bits(uint lbp, uint diameter){
  lbp <<= 1; 
  lbp |= (((unsigned int)1 << diameter) & lbp) >> diameter;
  return lbp;
}

uint LBPMap::lbp_encode(uint LBP[perimeter]){
  uint lbp_code = 0;
  for(int i = 0; i < perimeter; i++)
    lbp_code |= LBP[i] << i;
  // UNIFORM
  try{
    return lbp_code_table.at(lbp_code);
  }
  // NON-UNIFORM
  catch(const std::out_of_range& e){
    return lbp_code_table.at(-1);
  }
}
