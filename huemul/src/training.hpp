
#ifndef HUEMUL_TRAINING_H
#define HUEMUL_TRAINING_H

#include "common.hpp"

using namespace arma;


mat sigmoide(mat z);

mat obtenerThetaEntrenado(mat X, mat Y);

mat predecir(mat X_test, mat Theta);

/**
 * Reemplaza todos los valores de la matriz menores que eps por eps, y mayores
 * que 1-eps por 1-eps. Se suele usar esp = 1e-15
 */
mat clipMat(mat matrix, double eps);

double logloss(mat Y_pred, mat Y_true_clipped);

#endif
