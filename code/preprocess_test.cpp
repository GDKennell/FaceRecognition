#include "PGMImage.h"
#include <iostream>
#include <vector>
#include <utility>

using namespace std;
int main(){
	/*PGMImage test1("../images/facea1.pgm");
	test1.DoG(1, 2, EDGE_ACCUMULATE);
	test1.save("../testDoG_edge_accumulate.pgm");

	PGMImage test1("../images/facea1.pgm");
  test2.gamma_correct(.2);
	test2.DoG(1, 2, EDGE_ACCUMULATE);
	test2.save("../test_gamma_DoG_edge_accumulate.pgm");*/
	
  PGMImage test2("../images/facea1.pgm");
	test2.DoG(1, 2, MIRROR);
	test2.save("../testDoG_mirror.pgm");
	
  PGMImage test3("../images/facea1.pgm");
  test3.gamma_correct(.2);
	test3.DoG(1, 2, MIRROR);
	test3.save("../test_gamma_DoG_mirror.pgm");

	PGMImage test4("../images/facea1.pgm");
	test4.DoG(1, 2, MIRROR);
  test4.contrast_equalize();
	test4.save("../testDoG_mirror_contrast.pgm");
	
  PGMImage test5("../images/facea1.pgm");
  test5.gamma_correct(.2);
	test5.DoG(1, 2, MIRROR);
  test5.contrast_equalize();
	test5.save("../test_gamma_DoG_mirror_contrast.pgm");

	/*PGMImage test3("../images/facea1.pgm");
	test3.DoG(1, 2, NONE);
	test3.save("../testDoG_none.pgm");*/

	/*PGMImage test2("../images/facea1.pgm");
	test2.gaussian(3);
	test2.save("../testGaussian1.pgm");*/

	/*PGMImage test3("../images/facea1.pgm");
	test3.gaussian(5);
	test3.save("../testGaussian5.pgm");
	cout << "Saved." << endl;*/

  /*vector<pair<int, int>> pixels;
  for(int i = 0; i < 50; i++){
    pair<int,int> d(i, 25);
    pixels.push_back(d);
  }
  PGMImage zero_zero(70, 50, pixels);
  zero_zero.gaussian(5);
  zero_zero.save("../zero_zero.pgm");*/


	return 0;
}
