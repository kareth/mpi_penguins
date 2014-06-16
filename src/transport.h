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
  void StartTransport() { status_ = Status::kTransport; transporting_started_ = time(nullptr); }
  void StartUnload() { status_ = Status::kUnload; unloading_started_ = time(nullptr); }
  void SetIdle() {
    status_ = Status::kIdle;
    unloading_time_ = rand() % (Configuration::MaxUnloadingTime - Configuration::MinUnloadingTime + 1)
      + Configuration::MinUnloadingTime;
  }

  // Count transport time from StartTransport to now
  // and compare it with parametrized constant
  bool Arrived() { return time(nullptr) - transporting_started_ > Configuration::TransportTime; }

  // Count unload time from StartUnload to now
  // and compare it with preset random value
  bool Unloaded() { return time(nullptr) - unloading_started_ > unloading_time_; }

  bool WaitingForShips() { return status_ == Status::kShips; }
  bool WaitingForUnload() { return status_ == Status::kUnload; }
  bool WaitingForPorts() { return status_ == Status::kPort; }
  bool WaitingForTransport() { return status_ == Status::kTransport; }
  bool Idle() { return status_ == Status::kIdle; }


 private:
  Status status_ = Status::kIdle;
  time_t transporting_started_;
  time_t unloading_started_;
  time_t unloading_time_ = 1;
};

}  // namespace penguins

#endif  // TRANSPORT_H_
