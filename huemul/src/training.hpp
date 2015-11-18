
#ifndef HUEMUL_TRAINING_H
#define HUEMUL_TRAINING_H

#include <armadillo>

using namespace arma;

mat sigmoide(mat z);


mat obtenerThetaEntrenado(mat X, mat Y, float alpha);

double logloss(mat Y_pred, mat Y_true);

#endif
