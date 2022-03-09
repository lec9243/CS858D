#include <sdsl/bit_vectors.hpp>
#include <iostream>
#include "select_support_local.hpp"

using namespace std;
using namespace sdsl;

void select_support_local::checklinkingwork() {
  cout << "CHECK THIS PASS COMPILATION!!!!\n";
}

uint64_t select_support_local::select1(uint64_t i) {
  // cout << "CHECK length is correct: " << bv_length << "\n";
  // if i is greater than B total length or i is zero, error
  if ((i > (uint64_t)bv_length) || (!i)) {
    return (uint64_t)(0-1);
  }
  uint64_t max_rank = rv->rank1(bv_length-1);
  // if i is greater than max rank, error
  if (i > max_rank) {
    cout << "rank out of bound" << endl;
    exit(0);
  }
  uint64_t position = bv_length - 1;
  uint64_t max_pos = position;
  uint64_t min_pos = 0;
  while ((rv->rank1(position) != i) || (rv->rank1(position-1) != (i-1))) {
    // cout << position << "pos???" << endl;
    if ((position == 1) && (rv->rank1(position) == i)) {
      break;
    }
    if (rv->rank1(position) > i) {
      max_pos = position;
      position = position - (max_pos-min_pos)/2;
      // cout << position << " pos" << endl;
      // rv->print_rs_block();
    } else if (rv->rank1(position) < i) {
      // cout << 222 << endl;
      min_pos = position;
      position = position + (max_pos-min_pos)/2;
      // cout << position << " pos" << endl;

    } else {
      // cout << 333 << endl;
      // seems like some combination can be done here
      max_pos = position;
      position = position - (max_pos-min_pos)/2;
      // cout << position << " pos" << endl;

    }
  }
  return position;
}

uint64_t select_support_local::overhead() {
  return rv->overhead();
}

int select_support_local::save(string& fname) {
  rv->save(fname);
  return 0;
}

int select_support_local::load(string& fname) {
  rv->load(fname);
  return 0;
}
