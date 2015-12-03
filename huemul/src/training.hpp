#ifndef HUEMUL_TRAINING_H
#define HUEMUL_TRAINING_H

#include "common.hpp"

using namespace arma;

fmat sigmoide(const fmat& z);

fmat SGD(const fmat& X, const fmat& Y);
fmat GD(const fmat& X, const fmat& Y);

fmat SGD(const fmat& X, const fmat& Y, double alpha);
fmat GD(const fmat& X, const fmat& Y, double alpha);

fmat GD(const fmat& X, const fmat& Y, double alpha, fmat Theta, int iters);

fmat predict(const fmat& X_test, const fmat& Theta);

/**
 * Reemplaza todos los valores de la matriz menores que eps por eps, y mayores
 * que 1-eps por 1-eps. Se suele usar esp = 1e-15
 */
fmat clipMat(const fmat& matrix, double eps);

double logloss(const fmat& Y_pred, const fmat& Y_true);

#endif
