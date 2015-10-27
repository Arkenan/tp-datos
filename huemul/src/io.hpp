
#ifndef HUEMUL_IO_H
#define HUEMUL_IO_H

#include "core.hpp"


class Print: public Step {

public:
  Step& message(string);

private:
  string message_;
  virtual void doProcess_();
};

class Ungzip: public Step {
private:
  virtual void doProcess_();
};

#endif
