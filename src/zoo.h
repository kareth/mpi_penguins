#ifndef ZOO_H_
#define ZOO_H_

#include "communication.h"
#include "snow_manager.h"
#include "transport.h"
#include <set>

namespace penguins {

class Zoo {
 public:
  void Run();

 private:
  const static int kMaxZoo = 100;

  void MainLoop();
  void ReleaseResources();
  void RequestResources();
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

  // TODO aman move it, make it param, move to some class that allow checking MaxShips and so on
  int ships_ = 10;

  // TODO aman move it 
  int ship_request_timestamp_;

  Transport transport_;

  std::set<Message> ships_queue_;
};

}  // namespace penguins

#endif  // ZOO_H_
