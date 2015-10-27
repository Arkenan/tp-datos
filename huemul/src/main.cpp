#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>

#include <armadillo>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "common.hpp"
#include "core.hpp"
#include "io.hpp"

using namespace arma;
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


map<string, int> getLabelMap(vector<vector<string>> vec) {
  set<string> labels;
  for(auto &item : vec) {
    labels.insert(item[CATEGORY_COL]);
  }

  map<string, int> labelMap;
  int i = 0;
  for (auto &label : labels) {
    labelMap.emplace(label, i);
    i++;
  }
  return labelMap;
}

mat getLabels(vector<vector<string>> vec, map<string, int> labelsMap) {
  mat labels(vec.size(), 1);
  int counter = 0;
  for(auto &item : vec) {
    labels(counter, 0) = labelsMap.at(item[CATEGORY_COL]);
    counter++;
  }
  return labels;
}

mat getFeatures(vector<vector<string>> vec) {
  mat features(vec.size(), 4);

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

    counter++;
  }

  return features;
}

mat scaleFeatures(mat X, mat mu, mat sigma) {
  for (unsigned int i = 0; i < X.n_cols; ++i) {
    X.col(i) = (X.col(i) - mu(i))/sigma(i);
  }
  return X;
}

mat sigmoide(mat z) {
  return pow(1.0 + exp(-z), -1);
}

// mat trainLogReg() {
//   float alpha = 1;
//   for (int i = 1; i < 500; i++){
//     theta = theta - (alpha/m)*X.t()*(g(X*theta)-y);
//   }
// }

int main(int argc, char const *argv[]) {
  clock_t tStart = clock();

  if (argc < 3) {
    printf("Usage\n ./huemul train.csv.gz test.csv.gz\n");
    return 0;
  }

  vector<vector<string>> lines = getLines(argv[1]);

  printf("getLines X: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  map<string, int> labelsMap = getLabelMap(lines);

  mat y_train = getLabels(lines, labelsMap);

  mat X_train = getFeatures(lines);

  mat mu = mean(X_train); // media
  mat sigma = stddev(X_train); // desviacion estandar

  X_train = scaleFeatures(X_train, mu, sigma);
  X_train = join_rows(vec(X_train.n_rows).fill(1.0), X_train);

  printf("prepare X: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  lines = getLines(argv[2]);

  printf("getLines Y: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  DATES_COL = 1;
  mat X_test = getFeatures(lines);
  X_test = scaleFeatures(X_test, mu, sigma);
  X_test = join_rows(vec(X_train.n_rows).fill(1.0), X_train);

  printf("prepare Y: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  // tStart = clock();

  // y_train.save("foo.mat", csv_ascii);


  // Print()
  //   .message("Fooo")
  //   .setName("First Print")
  //   .then(&Print()
  //         .message("Bar")
  //         .setName("Second Print"))
  //   .then(&Print()
  //         .message("Baz")
  //         .setName("Third Print"))
  //   .process();

  return 0;
}
