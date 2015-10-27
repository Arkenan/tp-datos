
#include "io.hpp"


void Print::doProcess_() {
  printf("%s\n", message_.c_str());
};

Step& Print::message(string message) {
  message_ = message;
  return *this;
};


void Ungzip::doProcess_() {

}
