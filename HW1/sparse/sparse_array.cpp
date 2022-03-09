#include <sdsl/bit_vectors.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include "sparse_array.hpp"
#include <string>

using namespace std;
using namespace sdsl;

bit_vector sparse_array::bv;

void sparse_array::checker() {
  cout << "CHECK function is Linked!" << "\n";
  cout << "Bit Vector is: " << bv << "\n";
  cout << "Rank at Pos 4 is: " << rv->rank1(4) << "\n";
}


void sparse_array::create(uint64_t size) {
  bv_length = size;
  bv = bit_vector(size, 0);
  // cout << bv << endl;
  // rv = new rank_support_local(&bv, 1);
  // rv->build_table();
  // rv->print_rs_block();
  // sv = new select_support_local(rv);
}

void sparse_array::update_support() {
  rv = new rank_support_local(&bv, 1);
  rv->build_table();
  // rv->print_rs_block();
  sv = new select_support_local(rv);
  // cout << rv->rank1(bv_length-1) << endl;
}

void sparse_array::append(string elem, uint64_t pos) {
  // cout << "Before: " << sparse_array::bv << "\n";
  elements.push_back(elem);
  sparse_array::bv[pos] = 1;
  // cout << "After: " << sparse_array::bv << "\n";
  update_support();
}

bool sparse_array::get_at_rank(uint64_t r, string& elem) {
  uint64_t max_size = rv->rank1(bv_length-1);
  // cout << max_size << "\n";
  // cout << bv_length << "\n";
  bool ret = true;
  if (r == 0) {
    ret = false;
    cout << "RANK TOO SMALL, IMPOSSIBLE ELEMENT AT RANK 0\n";
  } else if (r > max_size) {
    ret = false;
  } else {
    elem = elements[r-1];
  }
  return ret;
}

bool sparse_array::get_at_index(uint64_t r, string& elem) {
  bool ret = false;
  cout << "rank at " << r << " is " << rv->rank1(r) << "\n";
  if (r == 0) {
    if (rv->rank1(r) == 1) {
      elem = elements[0];
      return true;
    } else {
      return false;
    }
  }
  // if ((r == 0) && (rv->rank1(r) == 1)) {
  //   elem = elements[0];
  //   ret = true;
  // } else {
  //   return false;
  // }
  if (rv->rank1(r) == rv->rank1(r-1)+1) {
    elem = elements[rv->rank1(r)-1];
    ret = true;
  }
  return ret;
}

uint64_t sparse_array::num_elem_at(uint64_t r) {
  return rv->rank1(r);
}

uint64_t sparse_array::size() {
  return bv_length;
}

uint64_t sparse_array::num_elem() {
  return rv->rank1(bv_length-1);
}

void sparse_array::save(string& fname) {

  ofstream myfile(fname);
  // cout << "HERE" << endl;
  bv.serialize(myfile);
  myfile.close();
  string fname1 = fname.append("_rv");
  // rv->print_rs_block();
  rv->save(fname1);
  // cout << "HEREEEEEEE" << endl;
  string fname2 = fname.append(".elements");
  ofstream output_file(fname2);
  ostream_iterator<string> output_iterator(output_file, "\n");
  copy(elements.begin(), elements.end(), output_iterator);
}

void sparse_array::load(string& fname) {
  // assume load is called when an empty sparse array is Instantiated
  // cout << "check load here works1\n";
  // update_support();
  // cout << "check load here works2\n";
  // cout << "check load here works3\n";
  // fname.append(".bit_vectors");
  ifstream myfile(fname);
  bv.load(myfile);
  myfile.close();
  rv = new rank_support_local(&bv, 1);
  string fname1 = fname.append("_rv");
  rv->load(fname1);
  rv->build_table_map();
  // cout << "HWHWHWHW" << endl;
  string fname2 = fname.append(".elements");
  ifstream input_file(fname2);
  cout << "HHHHHERE" << endl;
  string buffer_str;
  // Read the next line from File untill it reaches the end.
  while (getline(input_file, buffer_str)) {
    // Line contains string of length > 0 then save it in vector
    if(buffer_str.size() > 0) {
      elements.push_back(buffer_str);
    }
  }
  rv->print_rs_block();
  cout << elements[0] << "\n";
  cout << elements[1] << "\n";
  cout << elements[2] << "\n";
  // istream_iterator<string> start(input_file), end;
  // elements = vector<string>(start, end);
  // copy(elements.begin(), elements.end(),
  //      ostream_iterator<string>(start, "\n"));
  // copy(elements.begin(), elements.end(),
  //      ostream_iterator<string>(start, "\n"));
}
