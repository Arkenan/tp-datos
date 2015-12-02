

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
  D(train = getLines(argv[1], 100000  , false), "getLines train");
  D(test = getLines(argv[2], 100000, true), "getLines test");

  FeatureConverter converter(train, test);

  mat X_train;
  mat X_test;

  D(X_train = converter.getTrainFeatures(), "get X_train");
  D(X_test = converter.getTestFeatures(), "get X_test");

  mat y_train;
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
  mat Theta;
  // D(Theta = SGD(X_train, y_train, 1.0), "train Logistic Regression");
  D(Theta = GD(X_train, y_train, 1.0), "train Logistic Regression");

  mat result;
  D(result = predict(X_test, Theta), "prediction");


  cout << "rows "  << result.n_rows << endl;

  D(writeMatrix(result, categories.getLabels(), "output.csv.gz"), "write output");

  return 0;
}
