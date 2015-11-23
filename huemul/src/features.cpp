
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

  int i = labelsMap_.size();
  for (auto &label : labels) {
    if (labelsMap_.count(label) == 0) {
      labelsMap_.emplace(label, i);
      i++;
    }
  }

#ifndef NDEBUG
  // for (const auto &p : labelsMap_) {
  //     cout << p.first << " = " << p.second << '\n';
  // }
  cout << "Labels count: " << labelsMap_.size() << endl;
#endif

}

mat LabelBinarizer::transform(parsedStrings vec, int column, int features) {
  mat labels = zeros(vec.size(), features);
  int counter = 0;
  for(auto &item : vec) {
    labels(counter, labelsMap_.at(item[column])) = 1;
    counter++;
  }
  return labels;
}


mat LabelBinarizer::transform(parsedStrings vec, int column) {
  return transform(vec, column, labelsMap_.size());
}

map<string, int> LabelBinarizer::getLabels() {
  return labelsMap_;
}


vector<string> StreetBinarizer::getItem(string item) {
  vector<string> vec;
  int pos = item.find('/');
  if (pos != string::npos) {
    vec = getItem(item.substr(0, pos));
    if (pos + 1 < item.size()) {
      vec.push_back(getItem(item.substr(pos + 1, item.size()))[0]);
    }
  } else {
    istringstream iss(item);
    string token;
    string s = "";
    while ( getline(iss, token, ' ') ) {
      if (token.size() > 2 && !any_of(token.begin(), token.end(), ::islower)
                           && !all_of(token.begin(), token.end(), ::isdigit)) {
        s += token + " ";
      }
    }
    s.pop_back();
    vec.push_back(s);
  }
  return vec;
}


void StreetBinarizer::fit(parsedStrings vec, int column) {
  set<string> labels;
  for(auto &item : vec) {
    for(auto &label : getItem(item[column])) {
      labels.insert(label);
    }
  }

  int i = labelsMap_.size();
  for (auto &label : labels) {
    if (labelsMap_.count(label) == 0) {
      labelsMap_.emplace(label, i);
      i++;
    }
  }

#ifndef NDEBUG
  // for (const auto &p : labelsMap_) {
  //     cout << p.first << " = " << p.second << '\n';
  // }
  cout << "Labels count: " << labelsMap_.size() << endl;
#endif

}

sp_mat StreetBinarizer::transformSparse(parsedStrings vec, int column) {
  sp_mat labels(vec.size(), labelsMap_.size());
  int counter = 0;
  for(auto &item : vec) {
    for(auto &label : getItem(item[column])) {
      labels(counter, labelsMap_.at(label)) = 1;
    }
    counter++;
  }
  return labels;
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
  mat features(vec.size(), 13);
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
    features(counter, 4) = stof(item[dayOfWeekCol + 3]);
    features(counter, 5) = stof(item[dayOfWeekCol + 4]);

    features(counter, 6 + getSeason(month)) = 1;

    auto dayOfWeek = item[dayOfWeekCol];
    auto address = item[dayOfWeekCol + 2];

    int isWeekend = 0;
    if (dayOfWeek.compare("Saturday") == 0 || dayOfWeek.compare("Sunday") == 0) {
      isWeekend = 1;
    }
    features(counter, 10) = isWeekend;

    int isDayTime = 0;
    if (hour > 6 && hour < 20) {
      isDayTime = 1;
    }
    features(counter, 11) = isDayTime;

    int isIntersection = 0;
    if (address.find('/') != string::npos) {
      isIntersection = 1;
    }
    features(counter, 12) = isIntersection;

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

mat reduceDimensions(sp_mat X, int dimensions, string filename) {
  printf("Starting SVD...\n");
  clock_t tStart = clock();
  mat U;
  vec s;
  mat V;
  svds(U, s, V, X, dimensions, 0.3);  // SVD de las calles a 64 dimensiones

  printf("Finished SVD, %.2fs elapsed\n", (float)(clock() - tStart) / CLOCKS_PER_SEC);
  cout << "U size: " << U.n_rows << "x" << U.n_cols << endl;
  cout << "s size: " << s.n_rows << "x" << s.n_cols << endl;
  cout << "V size: " << V.n_rows << "x" << V.n_cols << endl;

  if (!filename.empty()) {
    U.save(filename);
  }
  return U;
}

mat reduceDimensions(sp_mat X, int dimensions) {
  return reduceDimensions(X, dimensions, "");
}

mat FeatureConverter::process(bool test) {

  int datesCol = 0, districtCol, dayOfWeekCol, streetCol;
  string filename;
  parsedStrings strings;
  if (test) {
    filename = "../data/testUsvd.csv";
    dayOfWeekCol = 1;
    districtCol = 2;
    streetCol = 3;
    strings = test_;
  } else {
    filename = "../data/trainUsvd.csv";
    dayOfWeekCol = 2;
    districtCol = 3;
    streetCol = 4;
    strings = train_;
    daysOfWeek_.fit(strings, dayOfWeekCol);
    districts_.fit(strings, districtCol);
    // streets_.fit(train_, 4);
    // streets_.fit(test_, 3);
  }

  mat features = getDateFeatures(strings, dayOfWeekCol);


  mat districts = districts_.transform(strings, districtCol);
  mat daysOfWeek = daysOfWeek_.transform(strings, dayOfWeekCol);
  // sp_mat streets = streets_.transformSparse(strings, streetCol);


  features.insert_cols(features.n_cols, districts);
  features.insert_cols(features.n_cols, daysOfWeek);

  // Esta parte tarda demasiado
  // mat U = reduceDimensions(streets, 128, filename);
  // mat U;
  // U.load(filename);
  // De esta manera se puede hacer pruebas con menos filas:
  // features.insert_cols(features.n_cols, U.rows(0, features.n_rows - 1));
  // features.insert_cols(features.n_cols, U);

  if (!test) {
    mu_ = mean(features.cols(0, 6));  // media
    sigma_ = stddev(features.cols(0, 6));  // desviacion estandar
  }

  // Escalo las features
  features = scaleFeatures(features, mu_, sigma_, 6);

  // Agrego bias
  features.insert_cols(0, colvec(features.n_rows).fill(1.0));

  // features.head_rows(50).raw_print();

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

mat scaleFeatures(mat X, mat mu, mat sigma, int columns) {
  for (unsigned int i = 0; i < columns; ++i) {
    X.col(i) = (X.col(i) - mu(i)) / sigma(i);
  }
  return X;
}

mat scaleFeatures(mat X, mat mu, mat sigma) {
  return scaleFeatures(X, mu, sigma, X.n_cols);
}
