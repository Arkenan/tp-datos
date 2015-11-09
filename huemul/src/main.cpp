

#include "common.hpp"
#include "core.hpp"
#include "io.hpp"
#include "features.hpp"


int main(int argc, char const *argv[]) {
  clock_t tStart = clock();

  if (argc < 3) {
    printf("Usage\n ./huemul train.csv.gz test.csv.gz\n");
    return 0;
  }

  vector<vector<string>> lines = getLines(argv[1]);

  printf("getLines X: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  map<string, int> labelsMap = getLabelMap(lines);

  mat y_train = getLabels(lines, labelsMap);

  mat X_train = getFeatures(lines);

  mat mu = mean(X_train); // media
  mat sigma = stddev(X_train); // desviacion estandar

  X_train = scaleFeatures(X_train, mu, sigma);
  X_train = join_rows(vec(X_train.n_rows).fill(1.0), X_train);

  printf("prepare X: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  lines = getLines(argv[2]);

  printf("getLines Y: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  mat X_test = getFeatures(lines);
  X_test = scaleFeatures(X_test, mu, sigma);
  X_test = join_rows(vec(X_train.n_rows).fill(1.0), X_train);

  printf("prepare Y: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  tStart = clock();

  y_train.save("foo.mat", csv_ascii);

  return 0;
}
