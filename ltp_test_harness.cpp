#include <cassert>
#include <deque>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "PGMImage.h"

using namespace std;

typedef deque<PGMImage> face_image_list_t;

deque<string> split_string(const string& str);

// Inputs test file where each line is a list of
// image file names corresponding to the same face

int main(int argc, char **argv) {
  assert(argc == 2);

  ifstream test_file;
  test_file.open(argv[1]);
  assert(test_file);

  deque<face_image_list_t> face_image_lists;
  
  deque<deque<string> >face_image_filenames;

  while(test_file) {
    string whole_line;
    getline(test_file, whole_line);
    deque<string> list = split_string(whole_line);
    if (list.size() < 2)
      break;
    face_image_filenames.push_back(list);
  }

  cout << "loading images" << endl;
  face_image_lists.resize(face_image_filenames.size());
  for (int i = 0; i < face_image_filenames.size(); ++i) {
    face_image_lists[i].resize(face_image_filenames[i].size());
    for (int j = 0; j < face_image_filenames[i].size(); ++j) {
      //cout << "loading image set: " << i << " filename: " << j << endl;
      //cout << "filename:\t" << face_image_filenames[i][j] << endl;
      face_image_lists[i][j].load(face_image_filenames[i][j]);
      //cout << "loaded. preprocessing..." << endl;
      face_image_lists[i][j].identity_preprocess();
      //cout << "preprocessed." << endl;
    }
  }
  cout << "images loaded" << endl;

  // intergroup_distances[i][j] represents the dstance between the ith and jth groups
  vector<vector<double>> intergroup_distances;
  intergroup_distances.resize(face_image_lists.size());
  for(auto i = intergroup_distances.begin(); i != intergroup_distances.end(); i++){
    i->resize(face_image_lists.size());
  }
 
  cout << "beginning comparisons" << endl;
  for(int i = 0; i < face_image_lists.size(); i++){
    for(int j = 0; j < face_image_lists.size(); j++){
      cout << "Computing distance between face sets " << i << " and " << j << endl;
      double total_distance_ij = 0;
      int comparisons = 0;
      for(int k = 0; k < face_image_lists[i].size(); k++){
        for(int l = 0; l < face_image_lists[j].size(); l++){
          if(!(i == j && k == l)){
            comparisons++;
            double d = face_image_lists[i][k].average_ltp_distance(face_image_lists[j][l]);
            cout << "distance(face[" << i << "][" << k << "], face[" << j << "][" << l << "]) =\t" << d << endl;
            total_distance_ij += d;
          }
        }
      }
      double average_distance_ij = total_distance_ij / comparisons;
      intergroup_distances[i][j] = average_distance_ij;
    }
  }
  cout << "comparisons finished." << endl;

  cout << "COMPARISON MATRIX:\n";
  for(int i = 0; i < face_image_lists.size(); i++){
    for(int j = 0; j < face_image_lists.size(); j++){
      printf("%0.2f ", intergroup_distances[i][j]);
    }
    cout << endl;
  }
}

deque<string> split_string(const string& str) {
  deque<string> return_deque;
  deque<int> space_locs;

  int loc = 0;
  space_locs.push_back(0);
  while(loc < str.size()) {
    loc = str.find(string(" "), loc + 1);
    if (loc == std::string::npos)
      loc = str.size();

    space_locs.push_back(loc);
  }
  
  for (int i = 0; i < space_locs.size() - 1; ++i) {
    string sub_str = str.substr(space_locs[i], space_locs[i+1] - space_locs[i]);
    if (sub_str[0] == ' ')
      sub_str = sub_str.substr(1,sub_str.size() - 1);
    if (sub_str.size())
      return_deque.push_back(sub_str);
  }
  return return_deque;
}

