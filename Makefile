all: ltp_test_harness.cpp PGMImage.cpp PGMImage.h ltp.cpp
	g++ ltp_test_harness.cpp PGMImage.cpp -g -std=c++11 -o LTP
