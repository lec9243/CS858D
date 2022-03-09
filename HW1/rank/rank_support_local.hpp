#ifndef RANKSUPPORT
#define RANKSUPPORT

#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <map>
// the code may be inspired by SDSL package, as suggested by instructor on Piazza
using namespace std;
using namespace sdsl;

class rank_support_local
{
public:
  // ok, a default constructor, but whatever I don't care
  rank_support_local() {}

  rank_support_local(bit_vector* v, int msg=0) {
    // ok, don't know why size_type is not in scope... expect definition in sdsl
    // int bv_length = v->bit_size();
    b = v;
    bv_length = v->bit_size();
    // int rs_block_size = (int)(pow(log2(bv_length), 2.0)/2.0);
    rs_block_size = (int)(pow((int)log2(bv_length), 2.0)/2.0);
    // int rb_block_size = (int)(log2(bv_length)/2.0);
    rb_block_size = (int)(log2(bv_length)/2.0);
    if (rs_block_size%rb_block_size) {
      int temp = rs_block_size/rb_block_size;
      rs_block_size = rb_block_size*(temp+1);
    }

    // error message for debugging
    if (msg) {
      cout << "bit vector size: " << bv_length << "\n";
      cout << "Rs block size: " << rs_block_size << "\n";
      cout << "Rb block size: " << rb_block_size << "\n";
    }
    // if B cannot be precisely divided, add 1 to max length
    // rs_block_max_size = bv_length/rs_block_size+bv_length%rs_block_size;
    rs_block_max_size = bv_length/rs_block_size;
    if (bv_length%rs_block_size) {
      rs_block_max_size = rs_block_max_size + 1;
    }
    // rb_block_max_size = bv_length/rb_block_size+bv_length%rb_block_size;
    rb_block_max_size = bv_length/rb_block_size;
    if (bv_length%rb_block_size) {
      rb_block_max_size = rb_block_max_size + 1;
    }

    rs_block = int_vector<>(rs_block_max_size, 0, rs_block_size);
    rb_block = int_vector<>(rb_block_max_size, 0, rb_block_size);
    rb_block_content = int_vector<>(rb_block_max_size, 0, rb_block_size);
  }
  // ok, a default destructor, but whatever I don't care again
  ~rank_support_local() {};
  bit_vector* b;
  // auto x = rank1(i);
  void build_table_map(int msg=0);
  void build_table(int msg=0);
  uint64_t rank1(uint64_t i);
  uint64_t overhead();
  int save(string& fname);
  int load(string& fname);
  void checkstructurework();
  void print_rs_block() {
    cout << "rs_b: " << rs_block << endl;
    cout << "rb_b: " << rb_block << endl;
  } ;

private:
  int bv_length;
  int rs_block_max_size;
  int rb_block_max_size;
  int rs_block_size;
  int rb_block_size;
  int_vector<> rs_block;
  int_vector<> rb_block;
  int_vector<> rb_block_content;
  map<int_vector<>, int_vector<>> table;
};
#endif
