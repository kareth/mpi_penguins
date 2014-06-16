#ifndef SNOW_MANAGER_H_
#define SNOW_MANAGER_H_

namespace penguins {

class SnowManager {
 public:
  // TODO aman move it, make it randomized
  int RequiredShips() { return 5; }

  int RequiredPorts() { return 4; }

  // TODO aman make it param or random - this is time between consecutive transports (one per year)
  bool NeedNow() { return !waiting_; }

  void Requested() { waiting_ = true; }
  void Received() { waiting_ = false; }

 private:
  bool waiting_ = false;

};

}  // namespace penguins

#endif  // SNOW_MANAGER_H_
