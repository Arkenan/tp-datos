
#include "training.hpp"


/* Logistic Regression con One vs. All */
float ALPHA = 1;


// Funcion sigmoidea
mat sigmoide(mat z) {
  return pow(1.0 + exp(-z), -1);
}

// X es la matriz de datos ([m casos] X [n features + BIAS])
// X ya se supone normalizada, y con la columna de BIAS agregada.
// Y es la matriz de respuestas ([m casos] X [c categorias posibles])
mat obtenerThetaEntrenado(mat X, mat Y){

  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles

  mat Theta(n,c);
  Theta.fill(1.0);

  for (int i = 1; i < 30; i++){
    /*
     * Gradient Descent para entrenar:
     * Aplica one versus all. Cada columna de Theta es un vector theta que le
     * otorga un peso a cada feature, ademas del BIAS.

     * Theta tiene tantas columnas como categorias posibles para clasificar,
     * de esta forma se entrena un vector theta para cada categoria.

     * La matriz Theta se va actualizando simultaneamente.
     * g(X*Theta) resulta en una matriz de m x c .
     * Y es la matriz de m x c que contiene las m respuestas representadas por
     * vectores de tamaño c.
    */
    Theta = Theta - (ALPHA / m) * X.t() * (sigmoide(X * Theta) - Y);

#ifndef DNDEBUG
    double loss = logloss(predecir(X, Theta), Y);
    printf ("terminada la iteración: %d, Logloss obtenido: %f \n", i, loss);
#endif
  }

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
    };
  };
  return Y_clipped;
}


double logloss(mat Y_pred, mat Y_true) {
  // Clip para mejorar logloss
  mat Y_pred_clipped = clipMat(Y_pred, pow(10, -15));
  return -sum(sum(Y_true % log(Y_pred_clipped))) / Y_pred.n_rows;
}
