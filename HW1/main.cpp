#include <iostream>
#include <ctime>
#include <sdsl/bit_vectors.hpp>
#include "rank/rank_support_local.hpp"
#include "select/select_support_local.hpp"
#include "sparse/sparse_array.hpp"

using namespace std;
using namespace sdsl;

int main(){

  // bit_vector b(16, 0);
  // b[0] = 1; b[3] = 1; b[5] = 1; b[6] = 1;
  // b[7] = 1; b[9] = 1; b[12]= 1; b[14]= 1;
  // rank_support_local r(&b);
  // r.build_table();
  // cout << "b: " << b << endl;
  // // r.checkstructurework();
  // for (int i = 0; i < 16; i++) {
  //   cout << "pos " << i << " has rank: " << r.rank1(i) << endl;
  // }
  // select_support_local s(&r);
  // cout << "The pos of 4th 1 is: " << s.select1(4) << endl;
  // // s.checklinkingwork();
  // cout << "r overhead(): " << r.overhead() << endl;
  // cout << "s overhead(): " << s.overhead() << endl;
  // string fn = "r_file";
  // r.save(fn);
  // string fns = "s_file";
  // s.save(fns);
  //
  // bit_vector b_temp(16);
  // rank_support_local r_temp(&b_temp);
  // cout << endl;
  // cout << "print loaded r vectors: " << endl;
  // r_temp.load(fn);
  //
  // select_support_local s_temp(&r_temp);
  // cout << "print loaded s vectors: " << endl;
  // s_temp.load(fns);
  //
  // bit_vector b2(256, 00);
  // b2[0] = 1;
  // // b2[67] = 1;  // bit_vector b(16, 0);
  // b2[135] = 1;
  // b2[254] = 1;
  // rank_support_local r2(&b2);
  // r2.build_table();
  // cout << endl;
  // cout << "At Pos 0, rank1 is: " << r2.rank1(0) << endl;
  // cout << "At Pos 1, rank1 is: " << r2.rank1(1) << endl;
  // cout << "At Pos 134, rank1 is: " << r2.rank1(134) << endl;
  // cout << "At Pos 135, rank1 is: " << r2.rank1(135) << endl;
  // cout << "At Pos 136, rank1 is: " << r2.rank1(136) << endl;
  // cout << "At Pos 253, rank1 is: " << r2.rank1(253) << endl;
  // cout << "At Pos 254, rank1 is: " << r2.rank1(254) << endl;
  // cout << "At Pos 255, rank1 is: " << r2.rank1(255) << endl;
  // select_support_local s2(&r2);
  // cout << "The pos of 2nd 1 is: " << s2.select1(2) << endl;
  // // cout << "The pos of 4th 1 is: " << s2.select1(4) << endl;
  // cout << "r2 overhead(): " << r2.overhead() << endl;
  //
  // bit_vector b3(10000000, 0);
  // b3[888] = 1;
  // rank_support_local r3(&b3);
  // r3.build_table();
  // cout << endl;
  // cout << "b3 at Pos 0, rank1 is: " << r3.rank1(0) << endl;
  // cout << "b3 at Pos 887, rank1 is: " << r3.rank1(887) << endl;
  // cout << "b3 at Pos 888, rank1 is: " << r3.rank1(888) << endl;
  // cout << "b3 at Pos 889, rank1 is: " << r3.rank1(889) << endl;
  // cout << "b3 at Pos 9999999, rank1 is: " << r3.rank1(9999999) << endl;
  // select_support_local s3(&r3);
  // cout << "The pos of 1st 1 is: " << s3.select1(1) << endl;
  // // cout << "The pos of 4th 1 is: " << s3.select1(4) << endl;
  // cout << "r3 overhead(): " << r3.overhead() << endl;
  //
  // sparse_array sa;
  // sa.create(50);
  // sa.append("a", 3);
  // sa.append("b", 4);
  // sa.append("c", 5);
  // sa.checker();
  // cout << "The overall bit vector length is: " << sa.size() << endl;
  // cout << "There are " << sa.num_elem() << " element(s)\n";
  // string elem_at_pos;
  // for (int i = 0; i < 8; i++) {
  //   if (sa.get_at_index(i, elem_at_pos)) {
  //     cout << "Element at position " << i << " is: " << elem_at_pos << "\n";
  //   } else {
  //     cout << "No Element at position " << i << "\n";
  //   }
  //   if (sa.get_at_rank(i, elem_at_pos)) {
  //     cout << "Element at Rank " << i << " is: " << elem_at_pos << "\n";
  //   } else {
  //     cout << "No Element at Rank " << i << "\n";
  //   }
  // }

  // string safile = "sa_temp";
  // sa.save(safile);
  // sparse_array sb;
  // sb.create(50);
  // safile = "sa_temp";
  // sb.load(safile);
  // cout << "The overall bit vector length is: " << sb.size() << endl;
  // cout << "There are " << sb.num_elem() << " element(s)\n";


        int n = 5;
        for (int i=0; i<10; i++) {
          n = n * 2;
          bit_vector bv(n, 0);
          cout << "---------- " << i <<": Test case " << n << " ----------" << endl;
          cout << "A bit vector of length "<< bv.size() << " was created." << endl;
          srand(13);
          for(int i=0; i<n*0.2; ++i){
                  bv[ rand()%n ] = 1;
          }
          cout << n*0.2 << " calls of rand() generated " << endl;
          rank_support_local rank_local(&bv);
          rank_local.build_table();
          cout << "---------- Rank ----------" << endl;
          auto start = chrono::system_clock::now();
          uint64_t rvalue = rank_local.rank1(bv.bit_size()-1);
          auto end = chrono::system_clock::now();
          chrono::duration<double> elapsed_seconds = end-start;
          cout << rvalue << " different numbers modulo " << n << endl;
          cout << "Rank_local overhead(): " << rank_local.overhead() << endl;
          cout << "Rank elapsed time: " << elapsed_seconds.count() << "s\n";

          select_support_local select_local(&rank_local);
          cout << "---------- Select ----------" << endl;
          auto start1 = chrono::system_clock::now();
          uint64_t svalue = select_local.select1(3);
          auto end1 = chrono::system_clock::now();
          chrono::duration<double> elapsed_seconds1 = end1-start1;
          cout << "The pos of 3th 1 is: " << svalue << endl;
          cout << "Select_local overhead(): " << select_local.overhead() << endl;
          cout << "Select elapsed time: " << elapsed_seconds1.count() << "s\n";
          cout << endl;
        }


  // following is for testing
  // bit_vector check0(1, 0);

}
