#ifndef ZOO_H_
#define ZOO_H_

#include "configuration.h"
#include "communication.h"
#include "snow_manager.h"
#include "transport.h"
#include "resource.h"
#include <set>

namespace penguins {

enum ResourceType {
  kShip,
  kPort
};
#define ResourceTypeCount 2

class Zoo {
 public:
  void Run();

 private:
  void MainLoop();

   void ProceedWithTransport();

  Communication communication_;

  Message requests_[4][Configuration::MaxZoo];

  Port port_;
  Ship ship_;

  SnowManager snow_manager_;

  int rank_;

  Transport transport_;

};

}  // namespace penguins

#endif  // ZOO_H_
