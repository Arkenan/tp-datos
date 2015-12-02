
#include "training.hpp"
// Cantidad de filas que toma SGD.
#define SGD_N 40
// Cantidad de iteraciones para gradient descent.
#define GD_IT 600

/* Logistic Regression con One vs. All */
float ALPHA = 1.0;


// Funcion sigmoidea
mat sigmoide(const mat& z) {
  return pow(1.0 + exp(-z), -1);
}

mat gdStep(const mat& Theta, const mat& X, const mat& Y, double alpha, double lambda) {
  mat gradient = (alpha / X.n_rows) * X.t() * (sigmoide(X * Theta) - Y);
  mat reg = (lambda / X.n_rows) * Theta;
  reg.row(0) = zeros<rowvec>(Y.n_cols);
  return Theta - gradient - reg;
}

// X es la matriz de datos ([m casos] X [n features + BIAS])
// X ya se supone normalizada, y con la columna de BIAS agregada.
// Y es la matriz de respuestas ([m casos] X [c categorias posibles])
mat SGD(const mat& X, const mat& Y, double alpha) {

  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles
  double lambda = 4.0;

  mat Theta(n, c);
  mat reg(n, c);
  mat gradient(n, c);
  Theta.fill(0.0);

  int its = m/SGD_N;
  mat subX, subY;
  double loss;

  for (int i = 0; i < GD_IT; i++){
    // SGD. Debería modularizar un poco esto. Quizás con un define.
    // cout << "iterancion " << i << endl;
    for (int j = 0; j < its; j++){
      subX = X.rows(SGD_N*j, SGD_N*(j+1)-1);
      subY = Y.rows(SGD_N*j, SGD_N*(j+1)-1);
      Theta = gdStep(Theta, subX, subY, alpha, lambda);
    }
    // Tomo las filas que faltan.

    subX = X.rows(its*SGD_N, m - 1);
    subY = Y.rows(its*SGD_N, m - 1);
    Theta = gdStep(Theta, subX, subY, alpha, lambda);

    cout << "terminada la iteración: %d" << i;
#ifndef NDEBUG
    if (i % 10 == 0) {
      loss = logloss(predict(X, Theta), Y);
      cout << " logloss %G" << loss;
    }
#endif
    cout << endl;
  }
  loss = logloss(predict(X, Theta), Y);
  cout << "Logloss final: " << loss << endl;

  return Theta;
}

mat SGD(const mat& X, const mat& Y) {
  return SGD(X, Y, ALPHA);
}

mat predict(const mat& X_test, const mat& Theta){
  mat resultado = sigmoide(X_test * Theta);
  return resultado;
}


mat clipMat(const mat& matrix, double eps) {
  // Clipping, no sé si es necesario
  int rows = matrix.n_rows;
  int cols = matrix.n_cols;
  mat Y_clipped(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      double element = matrix(i, j);
      if (eps < element) {
        if (element > 1 - eps) {
          Y_clipped(i, j) = 1 - eps;
        } else {
          Y_clipped(i, j) = element;
        }
      } else {
        Y_clipped(i, j) = eps;
      }
    };
  };
  return Y_clipped;
}

// Gradient descent clásico / en tanda.
mat GD(const mat& X, const mat& Y, double alpha) {

  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles
  double lambda = 4.0;
  double loss;

  cout << "Training dimensions: " << m << "x" << n << endl;

  mat Theta(n, c);
  mat reg(n, c);
  mat gradient(n, c);
  Theta.fill(0.0);

  for (int i = 0; i < GD_IT; i++){
    // cout << "iterancion " << i << endl;
    Theta = gdStep(Theta, X, Y, alpha, lambda);

    cout << "terminada la iteración " << i;
#ifndef NDEBUG
    if (i % 10 == 0) {
      loss = logloss(predict(X, Theta), Y);
      cout << " logloss " << loss;
    }
#endif
    cout << endl;
  }
  loss = logloss(predict(X, Theta), Y);
  cout << "Logloss final: " << loss << endl;

  return Theta;
}

mat GD(const mat& X, const mat& Y) {
  return GD(X, Y, ALPHA);
}

double logloss(const mat& Y_pred, const mat& Y_true) {
  // Clip para mejorar logloss
  mat Y_pred_clipped = clipMat(Y_pred, pow(10, -15));
  return -sum(sum(Y_true % log(Y_pred_clipped))) / Y_pred.n_rows;
}
