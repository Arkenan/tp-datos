

#include "common.hpp"
#include "core.hpp"
#include "io.hpp"
#include "features.hpp"
#include "training.hpp"

double timeDiff(double start) {
  return (clock() - start) / CLOCKS_PER_SEC;
};

#ifdef NDEBUG
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
  parsedStrings train;
  parsedStrings test;
  D(train = getLines(argv[1], false), "getLines train");
  D(test = getLines(argv[2], true), "getLines test");

  FeatureConverter converter(train, test);

  fmat X_train;
  fmat X_test;

  D(X_train = converter.getTrainFeatures(), "get X_train");
  D(X_test = converter.getTestFeatures(), "get X_test");

  fmat y_train;
  LabelBinarizer categories;

  D(
  categories.fit(train, 1);
  y_train = categories.transform(train, 1, 39),
  "get Y_train"
  );

  // Release memory
  printf("Releasing memory\n");
  parsedStrings().swap(train);
  parsedStrings().swap(test);

  // y_train.head_rows(50).raw_print();
  fmat Theta;
  // D(Theta = SGD(X_train, y_train, 1.0), "train Logistic Regression");
  D(Theta = GD(X_train, y_train, 1.0), "train Logistic Regression");

  fmat result;
  D(result = predict(X_test, Theta), "prediction");
  D(writeMatrix(result, categories.getLabels(), "output1200.csv.gz"), "write 1200 output");


  D(Theta = GD(X_train, y_train, 1.0, Theta, 200), "200 more iterations");
  D(result = predict(X_test, Theta), "prediction");
  D(writeMatrix(result, categories.getLabels(), "output1400.csv.gz"), "write 1400 output");


  D(Theta = GD(X_train, y_train, 1.0, Theta, 100), "100 more iterations");
  D(result = predict(X_test, Theta), "prediction");
  D(writeMatrix(result, categories.getLabels(), "output1500.csv.gz"), "write 1500 output");

  D(Theta = GD(X_train, y_train, 1.0, Theta, 100), "100 more iterations");
  D(result = predict(X_test, Theta), "prediction");
  D(writeMatrix(result, categories.getLabels(), "output1600.csv.gz"), "write 1600 output");

  return 0;
}
