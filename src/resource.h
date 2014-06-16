#ifndef PENGUINS_RESOURCE_H_
#define PENGUINS_RESOURCE_H_

#include <set>
#include "communication.h"
#include "configuration.h"

namespace penguins {

class Resource {
  public:
    typedef enum Type {
      kShip,
      kPort
    } Type;

  public:
    Resource(Type type, int amount) : amount_(amount), type_(type) { }

    void Request(int quantity, Communication& communication);
    void Release(int quantity, Communication& communication);
    void ProcessChanges(Communication& communication);
    void ReplyToRequests(Communication& communication);
    void ReplyQueuedMessages(Communication& communication);
    bool ShouldWaitFor(const Message& message, Communication& communication);

    int timestamp_;
    int amount_;
    Message* requests_[4];

  private:
    std::set<Message> queue_;
    Type type_;
};

class Ship : public Resource {
  public:
    Ship() : Resource(Resource::Type::kShip, Configuration::MaxShips) { }
};

class Port : public Resource {
  public:
    Port() : Resource(Resource::Type::kPort, Configuration::MaxPorts) { }
};

}

#endif
