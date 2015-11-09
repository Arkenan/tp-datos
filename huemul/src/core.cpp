
#include "core.hpp"

Step::Step(): name("Base step"), next_(NULL) {
};

Step::Step(string n): name(n), next_(NULL) {
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
  start_ = clock();
};

void Step::tearDown() {
  logElapsedTime();

  if (next_) {
    next_->process(this);
  }
};

void Step::process(Step* previous) {
  init();
  doProcess_(previous);
  tearDown();
};

void Step::process() {
  process((Step*) NULL);
}

void Step::logElapsedTime() {
  clock_t now = clock();
  double timeElapsed = (double)(now - start_) / CLOCKS_PER_SEC;
  printf("%s: %.4fs\n", name.c_str(), timeElapsed);
}

void Step::print() {}
