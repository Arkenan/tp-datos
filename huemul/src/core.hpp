#ifndef HUEMUL_CORE_H
#define HUEMUL_CORE_H

#include "common.hpp"


class Step {
public:
  string name;

  Step();
  Step(string n);

  Step& then(Step* next);

  Step& setName(string name);

  void init();
  void tearDown();

  void process();
  void process(Step* step);

  void logElapsedTime();
  void print();

private:
  Step* next_;
  virtual void doProcess_(Step* prev) = 0;

  clock_t start_;
};


#endif
