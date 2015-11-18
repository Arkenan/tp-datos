
#ifndef HUEMUL_TRAINING_H
#define HUEMUL_TRAINING_H

#include <armadillo>

using namespace arma;

mat sigmoide(mat z);

mat obtenerThetaEntrenado(mat X, mat Y);

mat predecir(mat X_test, mat Theta);

#endif
