#ifndef TRANSPORT_H_
#define TRANSPORT_H_

namespace penguins {

enum class Status {
  kIdle,
  kShips,
  kTransport,
  kPort,
  kUnload
};

class Transport {
 public:

  void WaitForPorts() { status_ = Status::kPort; }
  void WaitForShips() { status_ = Status::kShips; }
  void StartTransport() { status_ = Status::kTransport; }
  void StartUnload() { status_ = Status::kUnload; }
  void SetIdle() { status_ = Status::kIdle; }

  // Count transport time from StartTransport to now
  // and compare it with parametrized constant
  bool Arrived() { return true; }

  // Count unload time from StartUnload to now
  // and compare it with preset random value
  bool Unloaded() { return true; }

  bool WaitingForShips() { return status_ == Status::kShips; }
  bool WaitingForUnload() { return status_ == Status::kUnload; }
  bool WaitingForPorts() { return status_ == Status::kPort; }
  bool WaitingForTransport() { return status_ == Status::kTransport; }
  bool Idle() { return status_ == Status::kIdle; }


 private:
  Status status_ = Status::kIdle;
};

}  // namespace penguins

#endif  // TRANSPORT_H_
