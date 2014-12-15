all: code/ltp_test_harness.cpp code/PGMImage.cpp code/PGMImage.h code/LBPMap.h code/LBPMap.cpp
	g++ -g code/ltp_test_harness.cpp code/PGMImage.cpp code/LBPMap.cpp -std=c++11 -o LTP
