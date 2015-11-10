
#ifndef HUEMUL_FEATURES_H
#define HUEMUL_FEATURES_H

#include <armadillo>
#include <map>
#include <set>

#include "io.hpp"

using namespace arma;

map<string, int> getLabelMap(parsedStrings vec);
mat getLabels(parsedStrings vec, map<string, int> labelsMap);
mat getFeatures(parsedStrings vec, int datesCol);

mat scaleFeatures(mat X, mat mu, mat sigma);

#endif
