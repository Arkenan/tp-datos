
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
fmat getLabels(parsedStrings vec, map<string, int> labelsMap);

/**
 * Convierte el set de entrenamiento en forma de strings en una matriz de
 * numeros.
 */
fmat getFeatures(parsedStrings vec, int datesCol);

/**
 * Normaliza las columnas de la matrix X
 */
fmat scaleFeatures(fmat X, fmat mu, fmat sigma);

/**
 * Escala las primeras @columns
 */
fmat scaleFeatures(fmat X, fmat mu, fmat sigma, int colums);


/**
 * Mapea valores unicos del set de entrenamiento a una clase unívoca
 */
class LabelBinarizer {
    public:
      LabelBinarizer();

      /**
       * A partir de una columna del set, asigna a cada valor único una clase.
       * La clase es un número de 0 a N - 1, siendo N la cantidad de valores
       * únicos.
       */
      void fit(parsedStrings vec, int colnum);

      /**
       * Devuelve una matriz binária con cada fila correspondiente a la fila
       * del set, con un 1 en la columna correspondiente a la clase, y cero en
       * el resto.
       */
      fmat transform(parsedStrings vec, int colnum);

      /**
       * Idem anterior, pero se le puede especificar la cantidad de columnas.
       */
      fmat transform(parsedStrings vec, int column, int features);

      virtual string getItem(string item);

      map<string, int> getLabels();
    private:
      map<string, int> labelsMap_;

};


/**
 * Un binarizador para las direcciones. Hace un preprocesamiento al campo de
 * dirección sacando el/los nombres de calles unívocos.
 *
 * Por ejemplo:
 * OAK ST / LAGUNA ST -> [LAGUNA, OAK]
 * 600 Block of 47TH AV -> [47TH]
 */
class StreetBinarizer {
  public:
    void fit(parsedStrings vec, int column);
    sp_mat transformSparse(parsedStrings vec, int column);
  private:
    vector<string> getItem(string item);
    map<string, int> labelsMap_;
};


/**
 * Un binarizador para el X e Y.
 */
class PositionBinarizer:
  public LabelBinarizer {
  public:
    virtual string getItem(string item);
  private:
    map<string, int> labelsMap_;
};


/**
 * Genera las matrices de features para los sets de test y sets de
 * entrenamiento.
 *
 * Las features existentes son (por columna):
 * 0 -> Año
 * 1 -> Mes
 * 2 -> Día
 * 3 -> Hora
 * 4 -> X
 * 5 -> Y
 * 6 .. 9 -> Estación
 * 10 -> Si es fin de semana
 * 11 -> Si es de día
 * 12 -> Si es una intersección de calles
 * 13 .. 24 -> Distritos
 * 25 .. 31 -> Día de semana
 * 27 .. 159 -> Calles (Opcional)
 */
class FeatureConverter {
    public:
      FeatureConverter(parsedStrings train, parsedStrings test);
      fmat getTestFeatures();
      fmat getTrainFeatures();

    private:
      fmat process(bool test);
      LabelBinarizer years_;
      LabelBinarizer months_;
      LabelBinarizer days_;
      LabelBinarizer hours_;
      LabelBinarizer districts_;
      LabelBinarizer daysOfWeek_;
      StreetBinarizer streets_;
      PositionBinarizer Xs_;
      PositionBinarizer Ys_;
      parsedStrings train_;
      parsedStrings test_;

      fmat mu_;  // media
      fmat sigma_;  // desviación standard

};


#endif
