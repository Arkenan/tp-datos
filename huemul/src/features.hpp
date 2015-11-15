
#ifndef HUEMUL_FEATURES_H
#define HUEMUL_FEATURES_H

#include <armadillo>
#include <map>
#include <set>

#include "io.hpp"

using namespace arma;

/**
 * A partir del vector parseado del set de entrenamiento, genera un diccionario
 * de clases, donde a cada string de clase le asigna un numero unico.
 * La columna de clases por defecto es la segunda.
 * Por ejemplo:
 *
 * ID | Clase         {
 * 0  | Rojo            Azul: 0
 * 1  | Azul    =>      Rojo: 1
 * 2  | Rojo          }
 */
map<string, int> getLabelMap(parsedStrings vec);

/**
 * A partir del set de entrenamiento y el diccionario de clases, genera el
 * un vector de clases correspondientes a cada sample.
 */
mat getLabels(parsedStrings vec, map<string, int> labelsMap);

/**
 * Convierte el set de entrenamiento en forma de strings en una matriz de
 * numeros.
 */
mat getFeatures(parsedStrings vec, int datesCol);

/**
 * Normaliza las columnas de la matrix X
 */
mat scaleFeatures(mat X, mat mu, mat sigma);

#endif
