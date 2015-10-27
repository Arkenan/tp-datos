#ifndef HUEMUL_CORE_H
#define HUEMUL_CORE_H

#include "common.hpp"


class Step {
public:
  string name;

  Step();

  Step& then(Step* next);

  Step& setName(string name);

  void previous(Step& previous);

  void init();
  void tearDown();

  void process();

private:
  Step* next_;
  virtual void doProcess_() = 0;

  clock_t tStart_;
};

#endif
