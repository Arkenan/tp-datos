
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

  for (int i = 1; i < 10; i++){
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
  }

  return Theta;
}

mat predecir(mat X_test, mat Theta){
  mat resultado = sigmoide(X_test * Theta);
  return resultado;
}



double logloss(mat Y_pred, mat Y_true) {
  double eps = pow(1, -15);
  mat Y_clipped = max(min(Y_pred, eps), eps) / sum(Y_pred, 1);
  return -sum(sum(Y_true % log(Y_clipped)));
}
