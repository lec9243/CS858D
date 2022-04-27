echo "To save some time..."
echo "Clean up and Compile"
make clean
make

if [ $# -eq 0 ]; then
  echo "No Kmer Input"
  echo "./buildsa --preftab 4 CMSC858D_S22_Project2_sample/ecoli.fa ecoli.binary"
  ./buildsa CMSC858D_S22_Project2_sample/ecoli.fa ecoli_notab.binary
  echo "./querysa ecoli_notab.binary CMSC858D_S22_Project2_sample/query.fa naive ecoli_notab.naive"
  ./querysa ecoli_notab.binary CMSC858D_S22_Project2_sample/query.fa naive ecoli_notab.naive
  echo "./querysa ecoli_notab.binary CMSC858D_S22_Project2_sample/query.fa simpaccel ecoli_notab.simpaccel"
  ./querysa ecoli_notab.binary CMSC858D_S22_Project2_sample/query.fa simpaccel ecoli_notab.simpaccel
  diff -y -E -Z -b -w -B --suppress-common-line <(sort ecoli_notab.naive) <(sort CMSC858D_S22_Project2_sample/query_res.txt) > diff_notab.naive
  diff -y -E -Z -b -w -B --suppress-common-line <(sort ecoli_notab.simpaccel) <(sort CMSC858D_S22_Project2_sample/query_res.txt) > diff_notab.simpaccel
  diff --suppress-common-line -y ecoli_notab.naive ecoli_notab.simpaccel > diff_notab.between
  echo "Difference Stored to File"
else
  echo "Kmer is: " $1
  echo "./buildsa --preftab 4 CMSC858D_S22_Project2_sample/ecoli.fa ecoli.binary"
  ./buildsa --preftab $1 CMSC858D_S22_Project2_sample/ecoli.fa ecoli.binary
  echo "./querysa ecoli.binary CMSC858D_S22_Project2_sample/query.fa naive ecoli.naive"
  ./querysa ecoli.binary CMSC858D_S22_Project2_sample/query.fa naive ecoli.naive
  echo "./querysa ecoli.binary CMSC858D_S22_Project2_sample/query.fa simpaccel ecoli.simpaccel"
  ./querysa ecoli.binary CMSC858D_S22_Project2_sample/query.fa simpaccel ecoli.simpaccel
  diff -y -E -Z -b -w -B --suppress-common-line <(sort ecoli.naive) <(sort CMSC858D_S22_Project2_sample/query_res.txt) > diff.naive
  diff -y -E -Z -b -w -B --suppress-common-line <(sort ecoli.simpaccel) <(sort CMSC858D_S22_Project2_sample/query_res.txt) > diff.simpaccel
  diff --suppress-common-line -y ecoli.naive ecoli.simpaccel > diff.between
  echo "Difference Stored to File"
fi
echo Done...
