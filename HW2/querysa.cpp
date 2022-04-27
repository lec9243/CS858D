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
#include "utils.hpp"


using namespace std;
using namespace sdsl;

int main(int argc, char* argv[]) {
  string index;
  string queries;
  string query_mode;
  string output;
  if (argc == 5) {
    if (strcmp(argv[3], "naive") && strcmp(argv[3], "simpaccel") && strcmp(argv[3], "naiveaccel")) {
      cout << "Incorrect Flag\nShould be [naive] or [simpaccel] or [naiveaccel]" << endl;
      cout << "Get: " << argv[3] << endl;
      return -1;
    }
    index = argv[1];
    queries = argv[2];
    query_mode = argv[3];
    output = argv[4];
  } else {
    cout << "Incorrect Command Line Argument" << endl;
    cout << "Form should be:" << endl;
    cout << "./querysa index queries query-mode output" << endl;
    return -1;
  }

  ifstream binaryreader(index, ios::in | ios::binary);
  if (!binaryreader) {
    cout << "Fail to open" << endl;
    return -1;
  }
  cereal::BinaryInputArchive cerealinput(binaryreader);
  string toRead;
  cerealinput(toRead);
  binaryreader.close();

  vector<string> InputSegaments;
  // contents load from binary file
  string InputDNA;
  vector<int> InputSA;
  unordered_map<string, pair<int, int>> InputPrefixTable;
  int InputKmer = 0;

  stringstream ss(toRead);
  string tempholder;
  while (getline(ss, tempholder, '$')) {
    InputSegaments.push_back(tempholder);
  }

  InputDNA = InputSegaments[0];
  stringstream streamsa(InputSegaments[1]);
  while (getline(streamsa, tempholder, ',')) {
    InputSA.push_back(stoi(tempholder));
  }

  if (InputSegaments.size() > 2) {
    stringstream streammap(InputSegaments[3]);
    while (getline(streammap, tempholder, '>')) {
      if (tempholder.find(',') != string::npos) {
        stringstream tempstream(tempholder);
        string temp_map_content;
        vector<string> map_content;
        while (getline(tempstream, temp_map_content, ',')) {
          map_content.push_back(temp_map_content);
        }
        if (!InputKmer) {
          InputKmer = map_content[0].size();
        }
        pair<int, int> current_interval (stoi(map_content[1]), stoi(map_content[2]));
        InputPrefixTable[map_content[0]] = current_interval;
      }
    }
  }

  // read FASTA request file
  vector<string> Names;
  vector<string> Sequences;
  ReadFasta(queries, Names, Sequences);
  if (Names.size() != Sequences.size()) {
    cout << "Error at reading request files, comment and request amount mismatch" << endl;
    return -1;
  }

  int sa_size = InputSA.size();

  // clean up the file that need to write later
  ofstream clearFile;
  clearFile.open(output, ofstream::out | ofstream::trunc);
  clearFile.close();

  ofstream toWritetoFile;
  toWritetoFile.open(output, ios_base::app);

  auto start1 = chrono::system_clock::now();

  if ((query_mode == "simpaccel")) {
    cout << "MODE SIMPACCEL" << endl;
    string found_dna_sequence; // for debug, otherwise useless
    vector<int> pos_list;
    // handle request one by one
    for (int i = 0; i < Names.size(); i++) {
      pos_list.clear();
      int l = 0;
      int r = sa_size-1; // otherwise, SA outbound possible
      string dna_request = Sequences[i];
      found_dna_sequence = "";
      if (InputKmer) {
        string dna_request_first_k = dna_request.substr(0, InputKmer);
        pair<int, int> dna_first_k_range = InputPrefixTable[dna_request_first_k];
        l = dna_first_k_range.first;
        r = dna_first_k_range.second;
      }
      string str_l = InputDNA.substr(InputSA[l], dna_request.size());
      string str_r = InputDNA.substr(InputSA[r], dna_request.size());
      int lcp_l = LCP_calculation(str_l, dna_request);
      int lcp_r = LCP_calculation(str_r, dna_request);
      // l should be strictly smaller than r
      // if r = l + 1, it means not found
      int lcp_c;
      // binary search part
      while (l + 1 < r) {
        int c = l+(int)(r-l)/2;
        lcp_c = min(lcp_l, lcp_r);
        int sa_item = InputSA[c];
        string dna_c_ori = InputDNA.substr(sa_item, dna_request.size());
        string dna_c = InputDNA.substr(sa_item+lcp_c, dna_request.size()-lcp_c);
        string dna_request_accel = dna_request.substr(lcp_c, dna_request.size());
        if (dna_c == dna_request_accel) {
          found_dna_sequence = dna_c;
          pos_list.push_back(sa_item);
          int more_sa_idx = c - 1;
          int more_sa_val = -1;
          while (more_sa_idx >= 0) {
            more_sa_val = InputSA[more_sa_idx];
            string more_dna_c_accel = InputDNA.substr(more_sa_val+lcp_c, dna_request.size()-lcp_c);
            if (more_dna_c_accel == dna_request_accel) {
              pos_list.push_back(more_sa_val);
              more_sa_idx = more_sa_idx - 1;
            } else {
              more_sa_idx = (0-1);
            }
          }
          more_sa_idx = c + 1;
          while (more_sa_idx < InputSA.size()) {
            more_sa_val = InputSA[more_sa_idx];
            string more_dna_c_accel = InputDNA.substr(more_sa_val+lcp_c, dna_request.size()-lcp_c);
            if (more_dna_c_accel == dna_request_accel) {
              pos_list.push_back(more_sa_val);
              more_sa_idx = more_sa_idx + 1;
            } else {
              more_sa_idx = InputSA.size() + 1;
            }
          }
          l = r+1;
        } else if (dna_c > dna_request_accel) {
          r = c;
          // lcp_r = lcp_c;
          lcp_r = LCP_calculation(dna_c_ori, dna_request);
        } else {
          l = c;
          // lcp_l = lcp_c;
          lcp_l = LCP_calculation(dna_c_ori, dna_request);
        }
      }
      // special case for continuous not found and while loop is break
      // the left most or the right most may not be found
      if (l+1 == r) {
        string l_str = InputDNA.substr(InputSA[l], dna_request.size());
        string r_str = InputDNA.substr(InputSA[r], dna_request.size());
        if (l_str == dna_request) {
          pos_list.push_back(InputSA[l]);
        }
        if (r_str == dna_request) {
          pos_list.push_back(InputSA[r]);
        }
      }

      // // following just for write to file
      // string comment_name = Names[i];
      // int comment_length = comment_name.size();
      // comment_name = comment_name.substr(1, comment_length-1);
      // // following 5 lines for indentation only, check comment below
      // toWritetoFile << comment_name;
      // for (int space_writer = comment_name.size(); space_writer < 16; space_writer++) {
      //   toWritetoFile << " ";
      // }
      // toWritetoFile << pos_list.size() << "      ";
      // // not sure if indentation matters, the following will work
      // // toWritetoFile << comment_name << "  " << pos_list.size() << "      ";
      // for (int num_list_idx = 0; num_list_idx < pos_list.size(); num_list_idx++) {
      //   toWritetoFile << " " << pos_list[num_list_idx];
      // }
      // toWritetoFile << "" << endl;
      // // write to file end here

    }
  } else {
    cout << "MODE NAIVE" << endl;
    string found_dna_sequence; // for debug, otherwise useless
    vector<int> pos_list;
    for (int i = 0; i < Names.size(); i++) {
      pos_list.clear();
      int l = 0;
      int r = sa_size;
      string dna_request = Sequences[i];
      found_dna_sequence = "";
      if (InputKmer) {
        string dna_request_first_k = dna_request.substr(0, InputKmer);
        pair<int, int> dna_first_k_range = InputPrefixTable[dna_request_first_k];
        l = dna_first_k_range.first;
        r = dna_first_k_range.second;
      }
      // binary search part
      // l should be strictly smaller than r
      // if r = l + 1, it means not found
      while (l + 1 < r) {
        int c = l+(int)(r-l)/2;
        int sa_item = InputSA[c];
        string dna_c = InputDNA.substr(sa_item, dna_request.size());
        if (dna_c == dna_request) {
          found_dna_sequence = dna_c;
          pos_list.push_back(sa_item);
          int more_sa_idx = c - 1;
          int more_sa_val = -1;
          while (more_sa_idx >= 0) {
            more_sa_val = InputSA[more_sa_idx];
            string more_dna_c = InputDNA.substr(more_sa_val, dna_request.size());
            if (more_dna_c == dna_request) {
              pos_list.push_back(more_sa_val);
              more_sa_idx = more_sa_idx - 1;
            } else {
              more_sa_idx = (0-1);
            }
          }
          more_sa_idx = c + 1;
          while (more_sa_idx < InputSA.size()) {
            more_sa_val = InputSA[more_sa_idx];
            string more_dna_c = InputDNA.substr(more_sa_val, dna_request.size());
            if (more_dna_c == dna_request) {
              pos_list.push_back(more_sa_val);
              more_sa_idx = more_sa_idx + 1;
            } else {
              more_sa_idx = InputSA.size() + 1;
            }
          }
          l = r+1;
        } else if (dna_c > dna_request) {
          // it could be r = c
          // the following line should be theoretically correct
          // and the same time complexity
          r = r - (r-l)/2;
        } else {
          l = l + (r-l)/2;
        }
      }
      // special case for continuous not found and while loop is break
      // the left most or the right most may not be found
      if (l+1 == r) {
        string l_str = InputDNA.substr(InputSA[l], dna_request.size());
        string r_str;
        if (r == sa_size) {
          // possible out of bound... though not likely I believe
          r_str = "";
        } else {
          r_str = InputDNA.substr(InputSA[r], dna_request.size());
        }
        if (l_str == dna_request) {
          pos_list.push_back(InputSA[l]);
        }
        if (r_str == dna_request) {
          pos_list.push_back(InputSA[r]);
        }
      }

      // // following just for write to file
      // string comment_name = Names[i];
      // int comment_length = comment_name.size();
      // comment_name = comment_name.substr(1, comment_length-1);
      // // following 5 lines for indentation only, check comment below
      // toWritetoFile << comment_name;
      // for (int space_writer = comment_name.size(); space_writer < 16; space_writer++) {
      //   toWritetoFile << " ";
      // }
      // toWritetoFile << pos_list.size() << "      ";
      // // not sure if indentation matters, the following will work
      // //toWritetoFile << comment_name << "  " << pos_list.size() << "      ";
      // for (int num_list_idx = 0; num_list_idx < pos_list.size(); num_list_idx++) {
      //   toWritetoFile << " " << pos_list[num_list_idx];
      // }
      // toWritetoFile << "" << endl;
      // // write to file end here

    }
  }

  auto end1 = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds1 = end1-start1;
  cout << "------------------------------------------" << endl;
  cout << "Query amount: " << Names.size() << endl;
  cout << "Genome length / Suffix Array Size: " << sa_size << endl;
  cout << "Algorithm: " << query_mode << endl;
  if (InputKmer) {
    cout << "With Prefix Table" << endl;
  } else {
    cout << "Without Prefix Table" << endl;
  }
  cout << "Time for query: " << elapsed_seconds1.count() << "s\n";


  toWritetoFile.close();

  return 0;
}
