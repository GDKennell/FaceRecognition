all: ltp_test_harness.cpp PGMImage.cpp PGMImage.h LBPMap.h LBPMap.cpp
	g++ ltp_test_harness.cpp PGMImage.cpp LBPMap.cpp -std=c++11 -o LTP
