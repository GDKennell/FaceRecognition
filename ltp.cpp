
float identity(float x) {
  return x;
}

float (*gain_fn)(float) = &identity;

// Finds average euclidian distance between a given pattern in lbp1
// and the nearest matching pattern in lbp2
float total_lbp_distance(const PGMImg& lbp1, const PGMImg& lbp2);

// Input two image file paths, output similarity
int main(int argc, char** argv) {
  //calculate LTP's of each pixel in each image
  assert(argc == 3);

  PGMImg img1(argv[1]), img2(argv[2]);
  assert(img1 && img2);
  assert(img1.width() == img2.width() && img1.height() == img2.height());

  pair<PGMImg, PGMImg> img1_lbps = img1.lbps();
  pair<PGMImg, PGMImg> img2_lbps = img2.lbps();

  float total_dist_lbp1 = total_lbp_distance(img1_lbps.first, img2_lbps.first);
  float total_dist_lbp2 = total_lbp_distance(img1_lbps.second, img2_lbps.second);

  //optimal avg distance (given same image twice) == 0
  //pathological worst case average distance is approximately avg(width,height)/2

  // being generous with worst case for now, will need to tweak it,
  // especially once we start doing more interesting things than just
  // euclidian distance

  cout<<"Sum of total distances between the two images (drumroll please)\n\n...\n\n...\n\n"<<total_dist_lbp1 + total_dist_lbp2<<endl;
  return 0;
}

float total_lbp_distance(const PGMImg& lbp1, const PGMImg& lbp2) {
  //for each pixel in img1, search for matching code in img2
  //record distance between pixel in img1 and pixel w/ matching code in img2

  float total_distance = 0.0f;
  for (int x = 0; x < lbp1.width(); ++x) {
    for (int y = 0; y < lbp1.height(); ++y) {
      pair<int, int> match_loc = lbp2.nearest_lbp_match(x, y, lbp1(x,y));
      float dist = sqrt(exp(x - match_loc.first, 2) + exp(y - match_loc.second, 2));
      total_distance += gain_fn(dist);
    }
  }

  return total_distance;
}



