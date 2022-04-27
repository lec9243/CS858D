all: buildsa querysa

buildsa: buildsa.cpp utils.hpp
	g++ -std=c++11 -O3 -DNDEBUG -I ~/include -I ./cereal/include -L ~/lib \
	buildsa.cpp utils.hpp -o buildsa -lsdsl -ldivsufsort -ldivsufsort64

querysa: querysa.cpp utils.hpp
	g++ -std=c++11 -O3 -DNDEBUG -I ~/include -I ./cereal/include -L ~/lib \
	querysa.cpp utils.hpp -o querysa -lsdsl -ldivsufsort -ldivsufsort64

clean:
	rm -f *.o buildsa querysa
