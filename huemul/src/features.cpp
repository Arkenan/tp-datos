
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
    labels.insert(getItem(item[column]));
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

string LabelBinarizer::getItem(string item) {
  return item;
}

fmat LabelBinarizer::transform(parsedStrings vec, int column, int features) {
  fmat labels = zeros<fmat>(vec.size(), features);
  int counter = 0;
  for(auto &item : vec) {
    labels(counter, labelsMap_.at(getItem(item[column]))) = 1;
    counter++;
  }
  return labels;
}


fmat LabelBinarizer::transform(parsedStrings vec, int column) {
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
  int elems = 0;
  for(auto &item : vec) {
    for(auto &label : getItem(item[column])) {
      elems++;
    }
  }
  umat locations(2, elems);
  elems = 0;
  int counter = 0;
  for(auto &item : vec) {
    for(auto &label : getItem(item[column])) {
      locations(0, elems) = counter;
      locations(1, elems) = labelsMap_.at(label);
      elems++;
    }
    counter++;
  }
  mat values = ones(elems);
  sp_mat labels(true, locations, values, counter, labelsMap_.size());
  cout << "Sparse matrix size: " << labels.n_rows << "x" << labels.n_cols << endl;
  return labels;
}

string PositionBinarizer::getItem(string item) {
  int pos = item.find('.');
  return item.substr(0, pos + 4);
  // float val = stof(item);
  // float nearest = roundf(val * 100) / 100;
  // return to_string(nearest);
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

parsedStrings getDates(parsedStrings vec) {
  parsedStrings dates;
  for (auto &item : vec) {
    vector<string> row;
    auto date = item[0];
    // 2014-08-04 16:30:00
    row.push_back(date.substr(0, 4));  // year
    row.push_back(date.substr(5, 2));  // month
    row.push_back(date.substr(8, 2));  // day
    row.push_back(date.substr(11, 2));  // hour
    dates.push_back(row);
  }
  return dates;
}

fmat getScalarFeatures(parsedStrings vec, int dayOfWeekCol) {
  fmat features(vec.size(), 10);
  int counter = 0;

  for(auto &item : vec) {
    auto date = item[0];

    // 2014-08-04 16:30:00
    auto year = stoi(date.substr(0, 4));
    auto month = stoi(date.substr(5, 2));
    // auto day = stoi(date.substr(8, 2));
    auto hour = stoi(date.substr(11, 2));

    // features(counter, 0) = year;
    // features(counter, 1) = month;
    // features(counter, 2) = day;
    // features(counter, 3) = hour;
    // features(counter, 0) = stof(item[dayOfWeekCol + 3]);  // X
    // features(counter, 1) = stof(item[dayOfWeekCol + 4]);  // Y

    features(counter, 0 + getSeason(month)) = 1;

    auto dayOfWeek = item[dayOfWeekCol];
    auto address = item[dayOfWeekCol + 2];

    int isWeekend = 0;
    if (dayOfWeek.compare("Saturday") == 0 || dayOfWeek.compare("Sunday") == 0) {
      isWeekend = 1;
    }
    features(counter, 4) = isWeekend;

    int isDayTime = 0;
    if (hour > 6 && hour < 20) {
      isDayTime = 1;
    }
    features(counter, 5) = isDayTime;

    features(counter, 6) = (year < 2006) * 1;
    features(counter, 7) = (year < 2010) * 1;
    features(counter, 8) = (year < 2012) * 1;

    int isIntersection = 0;
    if (address.find('/') != string::npos) {
      isIntersection = 1;
    }
    features(counter, 9) = isIntersection;

    counter++;
  }

  return features;
}


FeatureConverter::FeatureConverter(parsedStrings train, parsedStrings test):
  train_(train), test_(test) {}

fmat FeatureConverter::getTrainFeatures() {
  return process(false);
};
fmat FeatureConverter::getTestFeatures() {
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
  cout << "Suma de las primeras filas de U " << sum(U.rows(0, 1)) << endl;
  cout << "Suma de las primeras columnas de U " << sum(U.cols(0, 1)) << endl;
  return U;
}

mat reduceDimensions(sp_mat X, int dimensions) {
  return reduceDimensions(X, dimensions, "");
}

fmat FeatureConverter::process(bool test) {

  int datesCol = 0, districtCol, dayOfWeekCol, streetCol, Xcol, Ycol;
  string filename;
  parsedStrings strings;
  parsedStrings dates;
  if (test) {
    filename = "../data/testUsvd.csv";
    dayOfWeekCol = 1;
    districtCol = 2;
    streetCol = 3;
    Xcol = 4;
    Ycol = 5;
    strings = test_;
    dates = getDates(test_);
  } else {
    filename = "../data/trainUsvd.csv";
    dayOfWeekCol = 2;
    districtCol = 3;
    streetCol = 4;
    Xcol = 5;
    Ycol = 6;
    strings = train_;
    cout << "Days of week" << endl;
    daysOfWeek_.fit(strings, dayOfWeekCol);
    cout << "Districts" << endl;
    districts_.fit(strings, districtCol);
    dates = getDates(train_);
    // streets_.fit(train_, 4);
    // streets_.fit(test_, 3);
    cout << "Xs" << endl;
    Xs_.fit(train_, 5);
    Xs_.fit(test_, 4);
    cout << "Ys" << endl;
    Ys_.fit(train_, 6);
    Ys_.fit(test_, 5);
    cout << "years" << endl;
    years_.fit(dates, 0);
    cout << "months" << endl;
    months_.fit(dates, 1);
    cout << "days" << endl;
    days_.fit(dates, 2);
    cout << "hours" << endl;
    hours_.fit(dates, 3);
  }

  fmat features = getScalarFeatures(strings, dayOfWeekCol);


  // sp_mat streets = streets_.transformSparse(strings, streetCol);

  // features.insert_cols(features.n_cols, years_.transform(dates, 0));
  features.insert_cols(features.n_cols, months_.transform(dates, 1));
  features.insert_cols(features.n_cols, days_.transform(dates, 2));
  features.insert_cols(features.n_cols, hours_.transform(dates, 3));
  features.insert_cols(features.n_cols, districts_.transform(strings, districtCol));
  features.insert_cols(features.n_cols, daysOfWeek_.transform(strings, dayOfWeekCol));
  features.insert_cols(features.n_cols, Xs_.transform(strings, Xcol));
  features.insert_cols(features.n_cols, Ys_.transform(strings, Ycol));

  // Esta parte tarda demasiado
  // fmat U = conv_to<fmat>::from(reduceDimensions(streets, 128, filename);

  // fmat U;
  // U.load(filename);
  // De esta manera se puede hacer pruebas con menos filas:
  // features.insert_cols(features.n_cols, U.rows(0, features.n_rows - 1));
  // features.insert_cols(features.n_cols, U);

  // if (!test) {
  //   mu_ = mean(features.cols(0, 2));  // media
  //   sigma_ = stddev(features.cols(0, 2));  // desviacion estandar
  // }

  // Escalo las features
  // features = scaleFeatures(features, mu_, sigma_, 2);

  // Agrego bias
  features.insert_cols(0, ones<fmat>(features.n_rows, 1));

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

fmat scaleFeatures(fmat X, fmat mu, fmat sigma, int columns) {
  for (unsigned int i = 0; i < columns; ++i) {
    X.col(i) = (X.col(i) - mu(i)) / sigma(i);
  }
  return X;
}

fmat scaleFeatures(fmat X, fmat mu, fmat sigma) {
  return scaleFeatures(X, mu, sigma, X.n_cols);
}
