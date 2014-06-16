#ifndef ZOO_H_
#define ZOO_H_

#include "configuration.h"
#include "communication.h"
#include "snow_manager.h"
#include "transport.h"
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
  const static int kMaxZoo = 100;

  void MainLoop();

  void Request(int quantity, ResourceType type);
  void Release(int quantity, ResourceType type);

  void ReplyToRequests();
  void ProceedWithTransport();
  void ReplyQueuedMessages();

  void ProcessChanges();

  bool ShouldWaitFor(const Message& message);

  Communication communication_;
  SnowManager snow_manager_;

  // TODO aman move it or so to manage 2 resources
  Message requests_[kMaxZoo];

  int rank_;

  int resource_amount_[ResourceTypeCount] = { Configuration::MaxShips, Configuration::MaxPorts };
  int resource_timestamp_[ResourceTypeCount];

  Transport transport_;

  std::set<Message> ships_queue_;
};

}  // namespace penguins

#endif  // ZOO_H_
