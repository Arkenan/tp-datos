
#include "training.hpp"
// Cantidad de filas que toma SGD.
#define SGD_N 40
// Cantidad de iteraciones para gradient descent.
#define GD_IT 10

/* Logistic Regression con One vs. All */
float ALPHA = 0.1;


// Funcion sigmoide.
mat sigmoide(mat z) {
  return pow(1.0 + exp(-z), -1);
}

// X es la matriz de datos ([m casos] X [n features + BIAS])
// X ya se supone normalizada, y con la columna de BIAS agregada.
// Y es la matriz de respuestas ([m casos] X [c categorias posibles])
mat SGD(mat X, mat Y){

  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles
  double lambda = 4.0;

  mat Theta(n, c);
  mat reg(n, c);
  mat gradient(n, c);
  Theta.fill(0.0);
    /*
     * Gradient Descent para entrenar:
     * Aplica one versus all. Cada columna de Theta es un vector theta que le
     * otorga un peso a cada feature, ademas del BIAS.

     * Theta tiene tantas columnas como categorias posibles para clasificar,
     * de esta forma se entrena un vector theta para cada categoria.

     * La matriz Theta se va actualizando simultaneamente.
     * sigmoide(X*Theta) resulta en una matriz de m x c .
     * Y es la matriz de m x c que contiene las m respuestas representadas por
     * vectores de tamaño c.
    */
  // Cantidad de iteraciones para recorrer la matriz una vez.
  int its = m/SGD_N;
  mat subX, subY;

  for (int i = 0; i < GD_IT; i++){
    // SGD. Debería modularizar un poco esto. Quizás con un define.
    cout << "iterancion " << i << endl;
    for (int j = 0; j < its; j++){
      subX = X.rows(SGD_N*j, SGD_N*(j+1)-1);
      subY = Y.rows(SGD_N*j, SGD_N*(j+1)-1);
      gradient = (ALPHA / SGD_N) * subX.t() * (sigmoide(subX * Theta) - subY);
      reg = (lambda / SGD_N) * Theta;
      reg.row(0) = zeros<rowvec>(c);
      Theta = Theta - gradient - reg;
    }
    // Tomo las filas que faltan.

    subX = X.rows(its*SGD_N, m - 1);
    subY = Y.rows(its*SGD_N, m - 1);
    gradient = (ALPHA / (m - its*SGD_N)) * subX.t() * (sigmoide(subX * Theta) - subY);
    reg = (lambda / (m - its*SGD_N)) * Theta;
    reg.row(0) = zeros<rowvec>(c);
    Theta = Theta - gradient - reg;
  }
#ifndef DNDEBUG
  double logLoss = logloss( sigmoide(X* Theta), Y);
  cout << "logloss "  << logLoss << endl;
#endif

  return Theta;
}

mat predecir(mat X_test, mat Theta){
  mat resultado = sigmoide(X_test * Theta);
  return resultado;
}


mat clipMat(mat matrix, double eps) {
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
    }
  }
  return Y_clipped;
}

// Gradient descent clásico / en tanda.
mat GD(mat X, mat Y){

  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles
  double lambda = 4.0;

  mat Theta(n, c);
  mat reg(n, c);
  mat gradient(n, c);
  Theta.fill(1.0);

  for (int i = 1; i < GD_IT; i++){
    cout << "iterancion " << i << endl;
    gradient = (ALPHA / m) * X.t() * (sigmoide(X * Theta) - Y);
    reg = (lambda / m) * Theta;
    reg.row(0) = zeros<rowvec>(c);
    Theta = Theta - gradient - reg;
  }

  return Theta;
}

double logloss(mat Y_pred, mat Y_true) {
  // Clip para mejorar logloss
  mat Y_pred_clipped = clipMat(Y_pred, pow(10, -15));
  return -sum(sum(Y_true % log(Y_pred_clipped))) / Y_pred.n_rows;
}
