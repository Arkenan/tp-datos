#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <map>
#include <set>

#include <armadillo>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "vendor/csv.h"

#include <time.h>

using namespace arma;
using namespace std;
using namespace boost;

int DATES_COL = 0;
int CATEGORY_COL = 1;


vector<vector<string>> getLines(std::string filename) {

  ifstream file(filename, ios_base::binary);
  iostreams::filtering_streambuf<iostreams::input> inbuf;
  inbuf.push(iostreams::gzip_decompressor());
  inbuf.push(file);

  istream stream(&inbuf);

  vector<vector<string>> vec;
  string headers;
  getline(stream, headers);

  cout << "Headers: " << headers << endl;

  vector<string> line;
  string token;

  int ch;
  while (EOF != (ch = stream.get())) {
    switch(ch) {
      case '\n':
          vec.push_back(line);
          line.clear();
          token.clear();
          break;
      case ',':
          line.push_back(token);
          token.clear();
          break;
      default:
          token.push_back(ch);
    }
  }

  return vec;
}


// Returns a map with categories as keys and their encodings as integer values
map<string, int> getEncodedLabels(vector<vector<string>> vec) {
  set<string> labels;
  for(auto &item : vec) {
    labels.insert(item[CATEGORY_COL]);
  }

  map<string, int> labelsMap;
  int i = 0;
  for (auto &label : labels) {
    labelsMap.emplace(label, i);
    i++;
  }
  return labelsMap;
}

mat getFeatures(vector<vector<string>> vec, map<string, int> labelsMap) {
  mat features(vec.size(), 5);

  int counter = 0;
  for(auto &item : vec) {
    auto date = item[DATES_COL];

    // 2014-08-04 16:30:00
    auto year = stoi(date.substr(0, 4));
    auto month = stoi(date.substr(5, 2));
    auto day = stoi(date.substr(8, 2));
    auto hour = stoi(date.substr(11, 2));

    features(counter, 0) = year;
    features(counter, 1) = month;
    features(counter, 2) = day;
    features(counter, 3) = hour;
    features(counter, 4) = labelsMap.at(item[CATEGORY_COL]);

    counter++;
  }

  return features;
}


int main(int argc, char const *argv[])
{
  clock_t tStart = clock();

  vector<vector<string>> vec = getLines(argv[1]);

  printf("getLines: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  map<string, int> labelsMap = getEncodedLabels(vec);

  mat train = getFeatures(vec, labelsMap);

  printf("getFeatures: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

  train.save("foo.mat", csv_ascii);

  for(auto &item : vec) {
    // copy(item.begin(), item.end(), ostream_iterator<string>(cout, "|"));
    // cout << endl;
  }

  return 0;
}
