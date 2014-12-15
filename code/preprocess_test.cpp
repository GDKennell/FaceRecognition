#include "PGMImage.h"
#include <iostream>

using namespace std;
int main(){
	PGMImage test1("../images/facea1.pgm");
	test1.save("../testFace.pgm");
	cout << "image loaded." << endl;
	test1.DoG(1, 2);
	cout << "DoG'd." << endl;
	test1.save("../testDoG.pgm");

	PGMImage test2("../images/facea1.pgm");
	test2.gaussian(.5);
	test2.save("../testGaussian1.pgm");

	PGMImage test3("../images/facea1.pgm");
	test3.gaussian(5);
	test3.save("../testGaussian5.pgm");
	cout << "Saved." << endl;
	return 0;
}
