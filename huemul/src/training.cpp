
#include "training.hpp"


/* Logistic Regression con One vs. All */

// Funcion sigmoidea
mat sigmoide(mat z) {
  return pow(1.0 + exp(-z), -1);
}

// X es la matriz de datos ([m casos] X [n features + BIAS])
// X ya se supone normalizada, y con la columna de BIAS agregada.
// Y es la matriz de respuestas ([m casos] X [c categorias posibles])
mat obtenerThetaEntrenado(mat X, mat Y, float alpha){

  int m = X.n_rows; // Filas = casos
  int n = X.n_cols; // Columnas = features + BIAS
  int c = Y.n_cols; // Categorias posibles

  mat Theta(n,c);
  Theta.fill(1.0);

  for (int i = 1; i < 500; i++){
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
    Theta = Theta - (alpha/m)*X.t()*(sigmoide(X*Theta)-Y);
  }

  return Theta;
}
