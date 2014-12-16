#include "PGMImage.h"
#include <iostream>
#include <vector>
#include <utility>

using namespace std;
int main(){
	/*PGMImage test1("../images/facea1.pgm");
	test1.save("../testFace.pgm");
	cout << "image loaded." << endl;
	test1.DoG(1, 2);
	cout << "DoG'd." << endl;
	test1.save("../testDoG.pgm");*/

	/*PGMImage test2("../images/facea1.pgm");
	test2.gaussian(3);
	test2.save("../testGaussian1.pgm");*/

	/*PGMImage test3("../images/facea1.pgm");
	test3.gaussian(5);
	test3.save("../testGaussian5.pgm");
	cout << "Saved." << endl;*/

  vector<pair<int, int>> pixels;
  for(int i = 0; i < 50; i++){
    pair<int,int> d(i, 25);
    pixels.push_back(d);
  }
  PGMImage zero_zero(70, 50, pixels);
  zero_zero.save("../zero_zero.pgm");

	return 0;
}
