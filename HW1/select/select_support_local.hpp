#ifndef SELECTSUPPORT
#define SELECTSUPPORT

#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include "rank/rank_support_local.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>

using namespace std;
using namespace sdsl;

class select_support_local
{
public:
  select_support_local() {}

  select_support_local(rank_support_local* r, int msg=0) {
    if (msg) {
      cout << "Error Message Requested!" << "\n";
    } else {
      // cout << "Error Message NOT required..." << "\n";
    }
    rv = r;
    bv_length = r->b->bit_size();
  }
  ~select_support_local() {}
  uint64_t select1(uint64_t i);
  uint64_t overhead();
  int save(string& fname);
  int load(string& fname);
  void checklinkingwork();

private:
  rank_support_local* rv;
  int bv_length;
};
#endif
