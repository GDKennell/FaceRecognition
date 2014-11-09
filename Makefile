all: ltp_test_harness.cpp PGMImage.cpp PGMImage.h ltp.cpp
	g++ ltp_test_harness.cpp PGMImage.cpp ltp.cpp -g -o LTP
