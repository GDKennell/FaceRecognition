all: ltp_test_harness.cpp PGMImage.cpp PGMImage.h LBPMap.h LBPMap.cpp
	g++ -g ltp_test_harness.cpp PGMImage.cpp LBPMap.cpp -std=c++11 -o LTP

preprocess: preprocess_test.cpp PGMImage.cpp PGMImage.h LBPMap.cpp LBPMap.h
	g++ PGMImage.cpp LBPMap.cpp preprocess_test.cpp -std=c++11 -g -o preprocess

clean:
	rm preprocess LTP
