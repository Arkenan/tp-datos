
#include "training.hpp"
// Cantidad de filas que toma SGD.
#define SGD_N 40
// Cantidad de iteraciones para gradient descent.
#define GD_IT 1200

/* Logistic Regression con One vs. All */
float ALPHA = 1.0;


// Funcion sigmoidea
fmat sigmoide(const fmat& z) {
  return pow(1.0 + exp(-z), -1);
}

fmat gdStep(const fmat& Theta, const fmat& X, const fmat& Y, double alpha, double lambda) {
  fmat gradient = (alpha / X.n_rows) * X.t() * (sigmoide(X * Theta) - Y);
  fmat reg = (lambda / X.n_rows) * Theta;
  reg.row(0) = zeros<frowvec>(Y.n_cols);
  return Theta - gradient - reg;
}

// X es la matriz de datos ([m casos] X [n features + BIAS])
// X ya se supone normalizada, y con la columna de BIAS agregada.
// Y es la matriz de respuestas ([m casos] X [c categorias posibles])
fmat SGD(const fmat& X, const fmat& Y, double alpha) {

  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles
  double lambda = 4.0;

  fmat Theta(n, c);
  fmat reg(n, c);
  fmat gradient(n, c);
  Theta.fill(0.0);

  int its = m/SGD_N;
  fmat subX, subY;
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

fmat SGD(const fmat& X, const fmat& Y) {
  return SGD(X, Y, ALPHA);
}

fmat predict(const fmat& X_test, const fmat& Theta){
  fmat resultado = sigmoide(X_test * Theta);
  return resultado;
}


fmat clipMat(const fmat& matrix, double eps) {
  // Clipping, no sé si es necesario
  int rows = matrix.n_rows;
  int cols = matrix.n_cols;
  fmat Y_clipped(rows, cols);
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

fmat GD(const fmat& X, const fmat& Y, double alpha, fmat Theta, int iters) {
  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles
  double lambda = 4.0;
  double loss;

  cout << "Training dimensions: " << m << "x" << n << endl;

  fmat reg(n, c);
  fmat gradient(n, c);

  for (int i = 0; i < iters; i++){
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

// Gradient descent clásico / en tanda.
fmat GD(const fmat& X, const fmat& Y, double alpha) {
  fmat Theta = zeros<fmat>(X.n_cols, Y.n_cols);
  return GD(X, Y, alpha, Theta, GD_IT);
}

fmat GD(const fmat& X, const fmat& Y) {
  return GD(X, Y, ALPHA);
}

double logloss(const fmat& Y_pred, const fmat& Y_true) {
  // Clip para mejorar logloss
  fmat Y_pred_clipped = clipMat(Y_pred, pow(10, -15));
  return -sum(sum(Y_true % log(Y_pred_clipped))) / Y_pred.n_rows;
}
