
#include "features.hpp"


using namespace boost;

int CATEGORY_COL = 1;
int DISTRICT_COL = 4;
int CANT_CATEGORIAS = 39;

LabelBinarizer::LabelBinarizer() {
}


void LabelBinarizer::fit(parsedStrings vec, int column) {
  set<string> labels;
  for(auto &item : vec) {
    labels.insert(item[column]);
  }

  int i = 0;
  for (auto &label : labels) {
    labelsMap_.emplace(label, i);
    i++;
  }

}

mat LabelBinarizer::transform(parsedStrings vec, int column, int features) {
  int size = vec.size();
  mat labels(size, features);
  int counter = 0;
  for(auto &item : vec) {
    rowvec respuestaBinarizada = rowvec(features);
    respuestaBinarizada.zeros();
    respuestaBinarizada.col( labelsMap_.at(item[column]) ) = 1;
    labels.row( counter ) = respuestaBinarizada;
    counter++;
  }
  return labels;
}

mat LabelBinarizer::transform(parsedStrings vec, int column) {
  return transform(vec, column, labelsMap_.size());
}


mat getDateFeatures(parsedStrings vec, int datesCol) {
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


FeatureConverter::FeatureConverter(parsedStrings train, parsedStrings test):
  train_(train), test_(test) {}

mat FeatureConverter::getTrainFeatures() {
  return process(false);
};
mat FeatureConverter::getTestFeatures() {
  return process(true);
};

mat FeatureConverter::process(bool test) {

  int datesCol = 0, districtCol;
  parsedStrings vec;

  if (test) {
    districtCol = 2;
    vec = test_;
  } else {
    districtCol = 3;
    vec = train_;
    districts_.fit(vec, districtCol);
  }

  mat features = getDateFeatures(vec, datesCol);

  mat districts = districts_.transform(vec, districtCol);

  features.insert_cols(features.n_cols, districts);

  if (!test) {
    mu_ = mean(features); // media
    sigma_ = stddev(features); // desviacion estandar
  }

  // Escalo las features
  features = scaleFeatures(features, mu_, sigma_);

  // Agrego bias
  features.insert_cols(0, colvec(features.n_rows).fill(1.0));

  return features;
}

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

  LabelBinarizer districts;

  mat features = getDateFeatures(vec, datesCol);

  return features;
}

mat scaleFeatures(mat X, mat mu, mat sigma) {
  for (unsigned int i = 0; i < X.n_cols; ++i) {
    X.col(i) = (X.col(i) - mu(i)) / sigma(i);
  }
  return X;
}
