#ifndef ZOO_H_
#define ZOO_H_

#include "communication.h"

namespace penguins {

class Zoo {
 public:
  void Run();

 private:
  Communication communication_;

};

}  // namespace penguins

#endif  // ZOO_H_
