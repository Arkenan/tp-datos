
#ifndef HUEMUL_FEATURES_H
#define HUEMUL_FEATURES_H

#include <armadillo>
#include <map>
#include <set>

#include "io.hpp"

using namespace arma;

map<string, int> getLabelMap(parsedStrings vec);
mat getLabels(parsedStrings vec, map<string, int> labelsMap);
mat getFeatures(parsedStrings vec);

mat scaleFeatures(mat X, mat mu, mat sigma);
mat sigmoide(mat z);
// mat trainLogisticRegression(mat X);

#endif
