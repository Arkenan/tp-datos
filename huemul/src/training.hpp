#ifndef HUEMUL_TRAINING_H
#define HUEMUL_TRAINING_H

#include "common.hpp"

using namespace arma;

mat sigmoide(const mat& z);

mat SGD(const mat& X, const mat& Y);
mat GD(const mat& X, const mat& Y);

mat SGD(const mat& X, const mat& Y, double alpha);
mat GD(const mat& X, const mat& Y, double alpha);

mat predict(const mat& X_test, const mat& Theta);

/**
 * Reemplaza todos los valores de la matriz menores que eps por eps, y mayores
 * que 1-eps por 1-eps. Se suele usar esp = 1e-15
 */
mat clipMat(const mat& matrix, double eps);

double logloss(const mat& Y_pred, const mat& Y_true);

#endif
