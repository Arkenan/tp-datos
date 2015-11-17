
#include "features.hpp"


using namespace boost;

int CATEGORY_COL = 1;
int CANT_CATEGORIAS = 39;


map<string, int> getLabelMap(parsedStrings vec) {
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

mat getLabels(parsedStrings vec, map<string, int> labelsMap) {
  mat labels(vec.size(), CANT_CATEGORIAS);
  int counter = 0;
  for(auto &item : vec) {
    rowvec respuestaBinarizada = rowvec(CANT_CATEGORIAS);
    respuestaBinarizada.zeros();
    respuestaBinarizada.col( labelsMap.at(item[CATEGORY_COL]) ) = 1;
    labels.row( counter ) = respuestaBinarizada;
    counter++;
  }
  return labels;
}

mat getFeatures(parsedStrings vec, int datesCol) {
  mat features(vec.size(), 4);

  int counter = 0;
  for(auto &item : vec) {
    auto date = item[datesCol];

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
    X.col(i) = (X.col(i) - mu(i)) / sigma(i);
  }
  return X;
}
