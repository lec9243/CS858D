all: main rank_support_local.o select_support_local.o

main: main.cpp rank_support_local.o select_support_local.o sparse_array.o
	g++ -std=c++11 -I ~/include -I. -L ~/lib main.cpp \
	rank_support_local.o select_support_local.o sparse_array.o -o \
	main -lsdsl -ldivsufsort -ldivsufsort64

rank_support_local.o: rank/rank_support_local.hpp rank/rank_support_local.cpp
	g++ -Wall -std=c++11 -I ~/include -L ~/lib -g -c rank/rank_support_local.cpp -o \
	rank_support_local.o -lsdsl -ldivsufsort -ldivsufsort64

select_support_local.o: select/select_support_local.hpp select/select_support_local.cpp
	g++ -Wall -std=c++11 -I ~/include -I. -L ~/lib -g -c select/select_support_local.cpp -o \
	select_support_local.o -lsdsl -ldivsufsort -ldivsufsort64

sparse_array.o: sparse/sparse_array.hpp sparse/sparse_array.cpp
	g++ -Wall -std=c++11 -I ~/include -I. -L ~/lib -g -c sparse/sparse_array.cpp -o \
	sparse_array.o -lsdsl -ldivsufsort -ldivsufsort64

test:
	g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib testsdsl.cpp -o \
	testsdsl -lsdsl -ldivsufsort -ldivsufsort64

clean:
	rm -f *.o main testsdsl
