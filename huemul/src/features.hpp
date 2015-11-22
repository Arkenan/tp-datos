
#ifndef HUEMUL_FEATURES_H
#define HUEMUL_FEATURES_H

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

/**
 * Escala las primeras @columns
 */
mat scaleFeatures(mat X, mat mu, mat sigma, int colums);

class LabelBinarizer {
    public:
      LabelBinarizer();

      void fit(parsedStrings vec, int colnum);
      mat transform(parsedStrings vec, int colnum);
      mat transform(parsedStrings vec, int column, int features);
      sp_mat transformSparse(parsedStrings vec, int column);

      map<string, int> getLabels();
    private:
      map<string, int> labelsMap_;

};


class FeatureConverter {
    public:
      FeatureConverter(parsedStrings train, parsedStrings test);
      mat getTestFeatures();
      mat getTrainFeatures();

    private:
      mat process(bool test);
      LabelBinarizer districts_;
      LabelBinarizer daysOfWeek_;
      LabelBinarizer streets_;
      parsedStrings train_;
      parsedStrings test_;

      mat mu_;  // media
      mat sigma_;  // desviación standard

};


#endif
