
#include "training.hpp"



mat sigmoide(mat z) {
  return pow(1.0 + exp(-z), -1);
}

// mat trainLogisticRegression(mat X) {
//   float alpha = 1;
//   for (int i = 1; i < 500; i++){
//     theta = theta - (alpha/m) * X.t() * (g(X * theta) - y);
//   }
// }
