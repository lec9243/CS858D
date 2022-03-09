#ifndef SPARSESUPPORT
#define SPARSESUPPORT

#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include "rank/rank_support_local.hpp"
#include "select/select_support_local.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <map>
#include <vector>

using namespace std;
using namespace sdsl;

class sparse_array
{
public:
  sparse_array() {
    cout << "CHECK Sparse Array is Instantiated" << "\n";
  }
  ~sparse_array() {}

  void create(uint64_t size);
  void append(string elem, uint64_t pos);
  bool get_at_rank(uint64_t r, string& elem);
  bool get_at_index(uint64_t r, string& elem);
  uint64_t num_elem_at(uint64_t r);
  uint64_t size();
  uint64_t num_elem();
  void save(string& fname);
  void load(string& fname);
  void checker();
  void printer();

private:
  uint64_t bv_length;
  static bit_vector bv;
  rank_support_local* rv;
  select_support_local* sv;
  vector<string> elements;
  void update_support();
};
#endif
