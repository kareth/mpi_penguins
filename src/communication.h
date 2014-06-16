#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <mpi.h>
#include "message.h"

namespace penguins {

enum class Tag {
  kRequest,
  kReply,
  kRelease,
  kAcquire
};

class Communication {
 public:
  Communication();
  void Receive(int source, Message* message, const Tag& tag);
  void ReceiveAll(Message* message, const Tag& tag);

  void Send(int dest, Message& message, const Tag& tag);
  void SendAll(Message& message, const Tag& tag);

  bool Test(int source, const Tag& tag);
  bool TestAll(const Tag& tag);

  int Rank();
  int Size();

  int Time() { return time_; };

 private:
  MPI::Request* Request(int source, const Tag& tag) { return &requests_[int(tag)][source]; }

  static const int kMaxTags = 4;
  static const int kMaxProcesses = 100;

  int time_ = 0;
  MPI::Request requests_[kMaxTags][kMaxProcesses];
};

}  // namespace penguins

#endif  // COMMUNICATION_H_
