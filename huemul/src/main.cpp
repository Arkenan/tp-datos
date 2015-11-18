

#include "common.hpp"
#include "core.hpp"
#include "io.hpp"
#include "features.hpp"
#include "training.hpp"

double timeDiff(double start) {
  return (clock() - start) / CLOCKS_PER_SEC;
};

#ifdef DNDEBUG
#define D(X, S) X
#else
#define D(X, S) {\
  printf("Starting %s...\n", S); \
  clock_t tStart = clock(); \
  X; \
  printf("Finished %s: %.2fs elapsed\n", S, timeDiff(tStart)); \
}
#endif


int main(int argc, char const *argv[]) {

  if (argc < 3) {
    printf("Usage\n ./huemul train.csv.gz test.csv.gz\n");
    return 0;
  }
  parsedStrings lines;
  D(lines = getLines(argv[1]), "getLines X");

  clock_t tStart = clock();
  map<string, int> labelsMap = getLabelMap(lines);

  mat y_train = getLabels(lines, labelsMap);

  mat X_train = getFeatures(lines, 0);

  mat mu = mean(X_train); // media
  mat sigma = stddev(X_train); // desviacion estandar

  X_train = scaleFeatures(X_train, mu, sigma);
  X_train = join_rows(vec(X_train.n_rows).fill(1.0), X_train);

  printf("prepare X_train, Y_train: %.2fs\n", timeDiff(tStart));
  tStart = clock();

  mat Theta =  obtenerThetaEntrenado(X_train, y_train);

  printf("train Logistic Regression: %.2fs\n", timeDiff(tStart));
  // Comienzo de lectura del set de test.
  tStart = clock();

  lines = getLines(argv[2]);

  printf("getLines X_test: %.2fs\n", timeDiff(tStart));
  tStart = clock();

  mat X_test = getFeatures(lines, 1);

  X_test = scaleFeatures(X_test, mu, sigma);
  X_test = join_rows(vec(X_test.n_rows).fill(1.0), X_test);

  printf("prepare Y: %.2fs\n", timeDiff(tStart));
  tStart = clock();

  mat respuesta = predecir(X_test, Theta);

  printf("prediccion: %.2fs\n", timeDiff(tStart));
  tStart = clock();
  writeMatrix(respuesta, labelsMap, "prueba.csv.gz");
  printf("writeMatrix: %.2fs\n", timeDiff(tStart));

  return 0;
}
