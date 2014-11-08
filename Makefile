all: PGMImage.cpp PGMImage.h ltp.cpp
	g++ PGMImage.cpp ltp.cpp -g -o LTP
