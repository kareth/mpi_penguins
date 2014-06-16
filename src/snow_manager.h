#ifndef SNOW_MANAGER_H_
#define SNOW_MANAGER_H_

#include "configuration.h"

namespace penguins {

class SnowManager {
 public:
  int RequiredShips() { return required_ships_; }

  int RequiredPorts() { return required_ports_; }

  // TODO aman make it param or random - this is time between consecutive transports (one per year)
  bool NeedNow() { return !waiting_; }

  void Requested() { waiting_ = true; }
  void Received() { waiting_ = false; }

  void Next() {
    required_ships_ = required_ports_ = (rand() % Configuration::MaxShips + 2) / 2;
  }

 private:
  bool waiting_ = false;
  int required_ships_;
  int required_ports_;

};

}  // namespace penguins

#endif  // SNOW_MANAGER_H_
