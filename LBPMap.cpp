#include "LBPMap.h"
#include "PGMImage.h"

#include <stdexcept>
#include <map>
using namespace std;
typedef unsigned int uint;

// Signal value for all non-uniform codes
const uint NON_UNIFORM_CODE = 0x100;

LBPMap& LBPMap::shared_map() {
  static LBPMap singleton_map;
  return singleton_map;
}

uint LBPMap::lbp_encode(uint LBP) {
  return lbp_code_table[LBP];
}

uint LBPMap::lbp_encode(uint LBP[perimeter]) {
  uint lbp_code = 0;
  for(int i = 0; i < perimeter; i++) {
    assert(LBP[i] == 0 || LBP[i] == 1);
    lbp_code |= LBP[i] << i;
  }
  return lbp_code_table[lbp_code];
}

uint LBPMap::lbp_decode(uint mapped_val) {
  for (int i = 0; i < MAX_CODE + 1; ++i) {
    if (lbp_code_table[i] == mapped_val)
      return i;
  }
  cerr<<"LBPMap: Could not find which LBP code is mapped to "<<mapped_val<<endl;
  exit(1);
  return 0;
}

LBPMap::LBPMap() {
  // All uniform codes and signal value for non-uniform codes
  // will be written over
  for (int i = 0; i < MAX_CODE + 1; ++i) {
    lbp_code_table[i] = -1;
  }

  uint* uniform_codes = all_uniform_codes();
  int count;

  for (count = 0; count < NUM_UNIFORM_CODES; ++count) {
    lbp_code_table[uniform_codes[count]] = count;
  }
  // This will only work if we keep my NON_UNIFORM_CODE
  // as MAX_CODE + 1, or as some arbitrary non-uniform code
  assert(NON_UNIFORM_CODE <= MAX_CODE + 1);
  lbp_code_table[NON_UNIFORM_CODE] = count; // NON-UNIFORM CODES
}

// Rotate bits one step to the left, with a given diameter. EG
// rotate_bits(001001, 4) returns 000011.
uint rotate_bits(uint lbp, uint diameter){
  lbp <<= 1; 
  lbp |= (((unsigned int)1 << diameter) & lbp) >> diameter;
  return lbp;
}

