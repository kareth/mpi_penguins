#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <mpi.h>

namespace penguins {

class Communication {
 public:
  static int Rank();
  static int Size();

 private:
};

}  // namespace penguins

#endif  // COMMUNICATION_H_
