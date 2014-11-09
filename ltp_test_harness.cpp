#include <cassert>
#include <deque>
#include <iostream>
#include <fstream>
#include <string>

#include "ltp.h"

using namespace std;

typedef deque<string> face_image_list_t;

deque<string> split_string(const string& str);

// Inputs test file where each line is a list of
// image file names corresponding to the same face

int main(int argc, char **argv) {
  assert(argc == 2);

  ifstream test_file;
  test_file.open(argv[1]);
  assert(test_file);

  deque<face_image_list_t> face_image_lists;

  while(test_file) {
    string whole_line;
    getline(test_file, whole_line);
    face_image_list_t list = split_string(whole_line);
    if (list.size() < 2)
      break;
    face_image_lists.push_back(list);
  }

  // Within groups
  float within_total_total_dist, within_total_avg_dist;
  int num_comparisons = 0;
  cout<<"calculating ltp average distance values for pairs of images of the same face"<<endl;
  for (int i = 0; i < face_image_lists.size(); ++i) {
    deque<string>& same_face_images = face_image_lists[i];
    for (int j = 0; j < same_face_images.size() - 1; ++j) {
      for (int k = j+1; k < same_face_images.size(); ++k) {
        ++num_comparisons;
        pair<float,float> comparison = ltp_comparison(same_face_images[j].c_str(),
                                                      same_face_images[k].c_str());
        within_total_total_dist += comparison.first;
        within_total_avg_dist += comparison.second;
      }
    }
    cout<<"  completed for combinations of face "<<i<<" / "<<face_image_lists.size()<<endl;
    cout<<"     - avg_dist: "<<within_total_avg_dist / (float)num_comparisons<<endl<<endl;
  }

  cout<<"In comparing images of the same person:"<<endl;
  cout<<"\tavg total: "<< within_total_total_dist / (float)num_comparisons<<endl;
  cout<<"\tavg avg: "<< within_total_avg_dist / (float)num_comparisons<<endl<<endl;

  cout<<"Computing ltp average distances between images of faces of different people"<<endl;
  // Between non-same faces
  float without_total_total_dist, without_total_avg_dist;
  num_comparisons = 0;
  for (int i = 0; i < face_image_lists.size(); ++i) {
    deque<string>& current_image_list = face_image_lists[i];
    for (int k = 0; k < face_image_lists.size(); ++k) {
      if (k == i) continue;
      deque<string>& different_image_list = face_image_lists[k];
      for (int j = 0; j < current_image_list.size(); ++j) {
        string current_image = current_image_list[j];
        for (int l = 0; l < different_image_list.size(); ++l) {
          ++num_comparisons;
          pair<float,float> comparison = ltp_comparison(current_image.c_str(),
                                                        different_image_list[l].c_str());
          without_total_total_dist += comparison.first;
          without_total_avg_dist += comparison.second;
        }
      }
      cout<<"  computed combinations of face "<<i<<" of "<<face_image_lists.size()<<" and face "<<k<<" of "<<face_image_lists.size()<<endl;
      cout<<"     - avg_dist: "<<without_total_avg_dist / (float)(num_comparisons)<<endl<<endl;
    }
  }
  cout<<"In comparing images of different people:"<<endl;
  cout<<"\tavg total: "<< without_total_total_dist / (float)num_comparisons<<endl;
  cout<<"\tavg avg: "<< without_total_avg_dist / (float)num_comparisons<<endl<<endl;

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

