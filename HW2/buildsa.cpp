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
#include <ctime>
#include "utils.hpp"

using namespace std;
using namespace sdsl;

int main(int argc, char* argv[]) {
  string reference;
  string output;
  int kmer = -1;
  if (argc == 3) {
    reference = argv[1];
    output = argv[2];
  } else if (argc == 5) {
    if (strcmp(argv[1], "--preftab")) {
      cout << "Incorrect Flag\nShould be:\n--preftab" << endl;
      cout << "Get: " << argv[1] << endl;
      return -1;
    }
    kmer = atol(argv[2]);
    reference = argv[3];
    output = argv[4];
  } else {
    cout << "Incorrect Command Line Argument" << endl;
    cout << "Form should be:" << endl;
    cout << "./buildsa [--preftab <k>] reference output" << endl;
    return -1;
  }

  // load from FASTA file
  // assume ONLY ONE Gene will be the input
  vector<string> requests;
  vector<string> DNAs;
  ReadFasta(reference, requests, DNAs);

  // build Suffix Array
  csa_bitcompressed<> sa;

  auto start1 = chrono::system_clock::now();

  construct_im(sa, DNAs[0], 1);

  auto end1 = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds1 = end1-start1;
  cout << "------------------------------------------" << endl;
  cout << "Time for build SA: " << elapsed_seconds1.count() << "s\n";


  int dna_size = DNAs[0].size();
  int sa_size = sa.size();

  // following for debugging, leave as comment

  // cout << "Examine the Suffix Array..." << endl;
  // for (int i = 0; i < 20; i++) {
  //   int sa_val = sa[i];
  //   cout << sa_val << endl;
  //   if (sa_val + 20 < dna_size) {
  //     cout << i << " : " << sa_val << " <= ";
  //     for (int k = sa_val; k < sa_val+20; k++) {
  //       cout << DNAs[0][k];
  //     }
  //     cout << endl;
  //   } else if (sa_val == dna_size) {
  //     cout << i << " : " << sa_val << " <= " << DNAs[0][sa_val] << "$" << endl;
  //   } else {
  //     cout << i << " : " << sa_val << " <= ";
  //     for (int k = sa_val; k < dna_size; k++) {
  //       cout << DNAs[0][k];
  //     }
  //     cout << endl;
  //   }
  // }
  // for (int i = 0; i < sa.size(); i++) {
  //   cout << sa[i] << "  ";
  // }
  // cout << " " << endl;
  // cout << "and that's it!" << endl;

  // build Prefix Table if required
  unordered_map<string, pair<int, int>> PrefixTable;

  auto start2 = chrono::system_clock::now();

  if (kmer > 0) {
    // this means --preftab flag is given
    string KPrefix = "";
    string prefix_holder = "";
    int begin_interval = -1;
    int end_interval = -1;
    for (int i = 0; i < sa.size(); i++) {
      int sa_val = sa[i];
      if (sa_val+kmer >= DNAs[0].size()) {
        if (end_interval > 0) {
          pair <int, int> current_interval (begin_interval, end_interval);
          PrefixTable[KPrefix] = current_interval;
        }
        continue;
      }
      prefix_holder = DNAs[0].substr(sa_val, kmer);
      //cout << "current prefix is: " << prefix_holder << endl;
      if (KPrefix == "") {
        // first time reached in iteration, special case
        KPrefix = prefix_holder;
        begin_interval = i;
        end_interval = i;
      }
      if (KPrefix != prefix_holder) {
        // prefix is changed
        pair <int, int> current_interval (begin_interval, end_interval);
        PrefixTable[KPrefix] = current_interval;
        KPrefix = prefix_holder;
        begin_interval = i;
        end_interval = i;
      } else {
        end_interval = end_interval + 1;
        // clean up
        prefix_holder = "";
      }
    }
    // clean up at the end, as the last is not written
    pair <int, int> current_interval (begin_interval, end_interval);
    PrefixTable[KPrefix] = current_interval;

    // following for debugging, leave as comment

    // cout << "check prefix table..." << endl;
    // cout << "PrefixTable size is: " << PrefixTable.size() << endl;
    // for (pair<string, pair<int, int>> element : PrefixTable) {
    //   cout << element.first << "  " << element.second.first << "," << element.second.second << endl;
    // }
    // cout << "prefix table check complete!" << endl;
  }

  auto end2 = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds2 = end2-start2;
  cout << "Time for build PrefixTable: " << elapsed_seconds2.count() << "s\n";

  cout << "------------------------------------------" << endl;
  
  // serialize and write to binary
  ofstream binarywriter(output, ios::out | ios::binary | ios::trunc);
  if (!binarywriter) {
    cout << "Fail to open" << endl;
    return -1;
  }
  cereal::BinaryOutputArchive cerealoutput(binarywriter);
  string toWrite = "";
  toWrite += DNAs[0];
  toWrite += "$";
  for (int t = 0; t < sa_size; t++) {
    int sa_item = sa[t];
    toWrite += to_string(sa_item);
    toWrite += ",";
  }
  if (kmer > 0) {
    toWrite += "$";
    toWrite += to_string(PrefixTable.size());
    toWrite += "$";
    for (pair<string, pair<int, int>> element : PrefixTable) {
      toWrite = toWrite + ">" + element.first + "," + to_string(element.second.first) + "," + to_string(element.second.second);
    }
  }
  cerealoutput(toWrite);
  binarywriter.close();

  return 0;
}
