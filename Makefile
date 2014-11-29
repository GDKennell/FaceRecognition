all: ltp_test_harness.cpp PGMImage.cpp PGMImage.h LBPMap.h LBPMap.cpp
	g++ -pg ltp_test_harness.cpp PGMImage.cpp LBPMap.cpp -g -std=c++11 -o LTP
