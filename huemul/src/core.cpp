
#include "core.hpp"

Step::Step(): name("Base step"), next_(NULL) {
};

Step& Step::then(Step* next) {
  if (next_) {
    next_->then(next);
  } else {
    next_ = next;
  }
  return *this;
};

Step& Step::setName(string name) {
  this->name = name;
  return *this;
}

void Step::init() {
  tStart_ = clock();
};

void Step::tearDown() {
  clock_t tStop = clock();
  double timeElapsed = (double)(tStop - tStart_) / CLOCKS_PER_SEC;
  printf("%s X: %.2fs\n", name.c_str(), timeElapsed);

  if (next_) {
    next_->process();
  }
};

void Step::process() {
  init();
  doProcess_();
  tearDown();
};
