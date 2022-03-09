#include <sdsl/bit_vectors.hpp>
#include <iostream>
#include <fstream>
#include "rank_support_local.hpp"

using namespace std;
using namespace sdsl;

void rank_support_local::build_table_map(int msg) {
  int table_size = (int)pow(rb_block_size, 2.0);
  for (int i = 0; i < table_size; i++) {
    int_vector<> key(1, i, rb_block_size);
    // the block size is NOT the most concat or accurate version
    // add 1 for ease here
    table[key] = int_vector<>(rb_block_size, 0, (int)log2(rb_block_size)+1);
    for (int j = 0; j < rb_block_size; j++) {
      for (int k = j; k < rb_block_size; k++) {
        table[key][k] = table[key][k]+((i>>(rb_block_size-1-j))&1);
      }
    }
  }
}

void rank_support_local::build_table(int msg) {
  // int table_size = (int)pow(rb_block_size, 2.0);
  int table_size = (int)pow(2.0, rb_block_size);
  for (int i = 0; i < table_size; i++) {
    int_vector<> key(1, i, rb_block_size);
    // the block size is NOT the most concat or accurate version
    // add 1 for ease here
    table[key] = int_vector<>(rb_block_size, 0, (int)log2(rb_block_size)+1);
    for (int j = 0; j < rb_block_size; j++) {
      for (int k = j; k < rb_block_size; k++) {
        table[key][k] = table[key][k]+((i>>(rb_block_size-1-j))&1);
      }
    }
  }
  //cout << 22222222222222222 << endl;
  uint64_t* data = b->data();
  // cout << "data[0]: " << (data[0]>>63&1) << "\n";
  // data is composed by reversed 64-bit block
  // don't say it is related to big or little endian...

  // build table
  int i, j;
  for (i = 0; i < bv_length; i++) {
    int data_block_pos = (int)i/64;
    int data_block_shift = i%64;
    int rs_block_pos = (int)i/rs_block_size;
    int rb_block_pos = (int)i/rb_block_size;
    int rb_block_up_limit = (rs_block_pos+1)*(rs_block_size/rb_block_size);
    if (rb_block_up_limit > rb_block_max_size) {
      rb_block_up_limit = rb_block_max_size;
    }
    int working_bit = (data[data_block_pos]>>data_block_shift)&1;
    // cout << i <<  "  "  << working_bit << " ? " <<data_block_pos << " "
    //      << rs_block_size << " <rs_block_size "
    //      << rb_block_size << " <rb_block_size "
    //      << rs_block_pos << " <rs_block_pos "
    //      << rs_block_max_size << " <rs_block_max_size "
    //      << rb_block_pos << " <rb_block_pos "
    //      << rb_block_max_size << " <rb_block_max_size "
    //      << rb_block_up_limit << "\n";
    if (rs_block_pos < rs_block_max_size-1) {
      if (working_bit) {
        for (j = rs_block_pos+1; j < rs_block_max_size; j++) {
          rs_block[j] = rs_block[j] + 1;
        }
      }
    }
    if (rb_block_pos < rb_block_max_size-1) {
      if (working_bit) {
        // cout << 111111111111111 << endl;
        for (j = rb_block_pos+1; j < rb_block_up_limit; j++) {
          rb_block[j] = rb_block[j] + 1;
        }
        // cout << 222222222222222 << endl;
      }
      rb_block_content[rb_block_pos] = (rb_block_content[rb_block_pos]<<1)|working_bit;
    } else if (rb_block_pos == rb_block_max_size - 1) {
      rb_block_content[rb_block_pos] = (rb_block_content[rb_block_pos]<<1)|working_bit;
    }
  }
  // error message for debugging
  if (msg) {
    cout << "check Rs array: \n";
    for (int i = 0; i < bv_length/rs_block_size; i++) {
      cout << "Rs pos " << i << " : " << rs_block[i] << "\n";
    }

    cout << "check Rb array: \n";
    for (int i = 0; i < bv_length/rb_block_size; i++) {
      cout << "Rb pos " << i << " : " << rb_block[i] << "\n";
    }

    cout << "check Rb content array: \n";
    for (int i = 0; i < bv_length/rb_block_size; i++) {
      cout << "Rb pos " << i << " : " << rb_block_content[i] << "\n";
    }

    cout << "check Table: \n";
    for (int i = 0; i < table_size; i++) {
      cout << "Table pos " << i << " : ";
      for (int j = 0; j < rb_block_size; j++) {
        int_vector<> key(1, i, rb_block_size);
        cout << table[key][j] << "  ";
      }
      cout << "\n";
    }
  }
}

uint64_t rank_support_local::rank1(uint64_t i) {
  if (i >= (uint64_t)bv_length) {
    cout << "index out of range" << endl;
    exit(0);
  }
  uint64_t ret = 0;
  int rs_pos = (int)i/rs_block_size;
  int rb_pos = (int)i/rb_block_size;
  int table_rank = i%rb_block_size;
  int_vector<> content(1, rb_block_content[rb_pos], rb_block_size);
  // following is for debugging
  // cout << "content   " << content;
  // cout << "for debug" << endl;
  // cout << "(* Rs value: " << rs_block[rs_pos] << " *)" << endl;
  // cout << "(* Rb value: " << rb_block[rb_pos] << " *)" << endl;
  // cout << "(* Table Content at Pos " << i << " : " << table[content] << " *)" << endl;
  ret = (uint64_t) rs_block[rs_pos]+rb_block[rb_pos]+table[content][table_rank];

  return ret;
}

uint64_t rank_support_local::overhead() {
    // cout << rs_block << " rs_block" << endl;
    // cout << rb_block << " rb block" << endl;
    // cout << rb_block_content << " rb_block_content" << endl;
    uint64_t bits = 0;
    int table_size = (int)pow(rb_block_size, 2.0);
    for (int i = 0; i < table_size; i++) {

      int_vector<> key(1, i, rb_block_size);
      bits = bits + key.size();
      bits = bits + table[key].size();
    }
    return bits + rs_block.size()+rb_block.size()+rb_block_content.size();
}

int rank_support_local::save(string& fname) {
  ofstream myfile(fname);
  rs_block.serialize(myfile);
  rb_block.serialize(myfile);
  rb_block_content.serialize(myfile);
  myfile.close();
  return 0;
}

int rank_support_local::load(string& fname) {
  ifstream myfile(fname);
  cout << fname << endl;
  rs_block.load(myfile);
  rb_block.load(myfile);
  rb_block_content.load(myfile);
  myfile.close();
  cout << "rs_block: " << rs_block << endl;
  cout << "rb_block: " << rb_block << endl;
  cout << "rb_block_content: " << rb_block_content << endl;
  return 0;
}

void rank_support_local::checkstructurework() {
  cout << "CHECK THIS IS REACHED!!!!\n";
}
