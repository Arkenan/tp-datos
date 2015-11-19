
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

  // for (const auto &p : labelsMap_) {
  //     std::cout << p.first << " = " << p.second << '\n';
  // }

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


int getSeason(int month) {
  int result = 0;  // Winter
  switch (month) {
    case 3: case 4: case 5:
      result = 1;  // Spring
      break;
    case 6: case 7: case 8:
      result = 2;  // Summer
      break;
    case 9: case 10: case 11:
      result = 3;  // Autumn
  }
  return result;
}

mat getDateFeatures(parsedStrings vec, int dayOfWeekCol) {
  mat features(vec.size(), 11);
  int counter = 0;
  for(auto &item : vec) {
    auto date = item[0];

    // 2014-08-04 16:30:00
    auto year = stoi(date.substr(0, 4));
    auto month = stoi(date.substr(5, 2));
    auto day = stoi(date.substr(8, 2));
    auto hour = stoi(date.substr(11, 2));

    features(counter, 0) = year;
    features(counter, 1) = month;
    features(counter, 2) = day;
    features(counter, 3) = hour;

    features(counter, 4 + getSeason(month)) = 1;

    auto dayOfWeek = item[dayOfWeekCol];
    auto address = item[dayOfWeekCol + 2];

    int isWeekend = 0;
    if (dayOfWeek.compare("Saturday") == 0 || dayOfWeek.compare("Sunday") == 0) {
      isWeekend = 1;
    }
    features(counter, 8) = isWeekend;

    int isDayTime = 0;
    if (hour > 6 && hour < 20) {
      isDayTime = 1;
    }
    features(counter, 9) = isDayTime;

    int isIntersection = 0;
    if (address.find('/') != string::npos) {
      isIntersection = 1;
    }
    features(counter, 10) = isIntersection;

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

  int datesCol = 0, districtCol, dayOfWeekCol;
  parsedStrings vec;

  if (test) {
    districtCol = 2;
    dayOfWeekCol = 1;
    vec = test_;
  } else {
    dayOfWeekCol = 2;
    districtCol = 3;
    vec = train_;
    daysOfWeek_.fit(vec, dayOfWeekCol);
    districts_.fit(vec, districtCol);
  }

  mat features = getDateFeatures(vec, dayOfWeekCol);

  // features.head_rows(50).raw_print();

  mat districts = districts_.transform(vec, districtCol);
  mat daysOfWeek = daysOfWeek_.transform(vec, dayOfWeekCol);

  // features.insert_cols(features.n_cols, districts);
  features.insert_cols(features.n_cols, daysOfWeek);

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
