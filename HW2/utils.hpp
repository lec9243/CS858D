#include <sdsl/bit_vectors.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <cstdlib>

using namespace std;

int LCP_calculation(string &s1, string &s2) {
  int length = min(s1.size(), s2.size());
  int ret = 0;
  for (int i = 0; i < length; i++) {
    if (s1[i] != s2[i]) {
      ret = i;
      break;
    }
  }
  return ret;
}

void ReadFasta(string &file, vector<string> &comments, vector<string> &DNA) {
  // cout << "address in function" << endl;
  // cout << &file << " " << &comments << " " << &DNA << endl;

  // might be too close to stackoverflow, not sure

  ifstream input(file);
  if (!input.good()) {
    cout << "Fail to open, incorrect file name" << endl;
    exit(1);
  }
  string line;
  string DNApart;
  while (getline(input, line)) {
    if (line.empty()) {
      continue;
    }
    if (line[0] == '>') {
      // cout << line << endl;
      comments.push_back(line);
      if (!DNApart.empty()) {
        DNA.push_back(DNApart);
        DNApart.clear();
      }
    } else {
      DNApart += line;
    }
  }
  if (!DNApart.empty()) {
    DNA.push_back(DNApart);
  }
  input.close();
  return;
}
