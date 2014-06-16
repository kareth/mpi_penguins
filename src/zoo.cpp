#include "zoo.h"

#include <cstdio>
#include <unistd.h>
#include <time.h>

namespace penguins {

void Zoo::Run() {
  rank_ = communication_.Rank();

  communication_.ReceiveAll(requests_, Tag::kRequest);
  communication_.ReceiveAll(requests_, Tag::kReply);
  MainLoop();
}

void Zoo::MainLoop() {
  while (true) {
    ProcessChanges();
    ProceedWithTransport();    // Once we got access to ships

    // unloading
    usleep(100000 * (rand() % 10 + 1));

    ReplyToRequests();
  }
}

void Zoo::ProcessChanges() {
  for (int i = 0; i < communication_.Size(); i++) {
    if (i != rank_ && communication_.Test(i, Tag::kRelease) && requests_[i].field(Field::kType) != -1) {
      resource_amount_[requests_[i].field(Field::kType)] += requests_[i].field(Field::kQuantity);
      communication_.Receive(i, &requests_[i], Tag::kRelease);
    }
  }

  for (int i = 0; i < communication_.Size(); i++) {
    if (i != rank_ && communication_.Test(i, Tag::kAcquire) && requests_[i].field(Field::kType) != -1) {
      resource_amount_[requests_[i].field(Field::kType)] -= requests_[i].field(Field::kQuantity);
      communication_.Receive(i, &requests_[i], Tag::kAcquire);
    }
  }
}

void Zoo::ReplyToRequests() {
  for (int i = 0; i < communication_.Size(); i++) {
    if (i != rank_ && communication_.Test(i, Tag::kRequest)) {
      //printf("%d received request from %d\n",rank_, requests_[i].field(Field::kRank));

      if (ShouldWaitFor(requests_[i])) {
        //printf("%d Sending ACC to %d\n", rank_, requests_[i].field(Field::kRank));
        // He is first...
        Message reply(rank_, requests_[i].field(Field::kType));
        communication_.Send(requests_[i].field(Field::kRank), reply, Tag::kReply);
      } else {
        // Wait for my acquire first
        //printf("%d has priority over %d\n", rank_, requests_[i].field(Field::kRank));
        ships_queue_.insert(requests_[i]);
      }

      communication_.Receive(i, &requests_[i], Tag::kRequest);
    }
  }
}

void Zoo::ReplyQueuedMessages() {
  //printf("replying to %d queued guys\n", ships_queue_.size());
  while (ships_queue_.size() > 0) {
    auto msg = *ships_queue_.begin();
    ships_queue_.erase(ships_queue_.begin());

    Message reply(rank_, msg.field(Field::kType));
    communication_.Send(msg.field(Field::kRank), reply, Tag::kReply);
    //printf("%d Sending ACC to %d\n", rank_, msg.field(Field::kRank));
  }
}

bool Zoo::ShouldWaitFor(const Message& message) {
  if (resource_timestamp_[message.field(Field::kType)] > message.field(Field::kTimestamp)) return true;
  if (resource_timestamp_[message.field(Field::kType)] < message.field(Field::kTimestamp)) return false;
  return rank_ < message.field(Field::kRank);
}

void Zoo::ProceedWithTransport() {
  if (transport_.Idle()) {
    snow_manager_.Next();
    printf("Zoo no. %d requests %d ship\n", rank_, snow_manager_.RequiredShips());
    Request(snow_manager_.RequiredShips(), ResourceType::kShip);
    transport_.WaitForShips();
  }
  else if (transport_.WaitingForShips()) {
    if (communication_.TestAll(Tag::kReply)) {
      // TODO FIXME
      // This shit is important. Its a bug right now. We're not guaranteed
      // that we will receive all change messages before replies.
      // Safest way would be probably to put acquire tag together with reply tag
      // and differentiate them just by some params, but that looks bad
      ProcessChanges();

      // Important:
      // If there is not enough ships, just wait, they will release soon
      if (resource_amount_[ResourceType::kShip] < snow_manager_.RequiredShips())
        return;
      // check for ships overflow
      if (resource_amount_[ResourceType::kShip] > Configuration::MaxShips)
        return;

      printf("Zoo no. %d acquired %d ships!\n", rank_, snow_manager_.RequiredShips());
      Message acquire(rank_, time(nullptr), snow_manager_.RequiredShips(), ResourceType::kShip);
      communication_.SendAll(acquire, Tag::kAcquire);

      resource_amount_[ResourceType::kShip] -= snow_manager_.RequiredShips();

      ReplyQueuedMessages();

      transport_.StartTransport();
    }
  }
  else if (transport_.WaitingForTransport()) {
    if (transport_.Arrived()) {
      printf("Zoo no. %d requests %d ports\n", rank_, snow_manager_.RequiredPorts());

      Request(snow_manager_.RequiredPorts(), ResourceType::kPort);

      transport_.WaitForPorts();
    }
  }
  else if (transport_.WaitingForPorts()) {
    if (communication_.TestAll(Tag::kReply)) {
      ProcessChanges();
      if (resource_amount_[ResourceType::kPort] < snow_manager_.RequiredPorts())
        return;
      if (resource_amount_[ResourceType::kPort] > Configuration::MaxPorts)
        return;

      printf("Zoo no. %d acquired %d ports!\n", rank_, snow_manager_.RequiredPorts());
      Message acquire(rank_, time(nullptr), snow_manager_.RequiredPorts(), ResourceType::kPort);
      communication_.SendAll(acquire, Tag::kAcquire);

      resource_amount_[ResourceType::kPort] -= snow_manager_.RequiredPorts();

      ReplyQueuedMessages();

      transport_.StartUnload();
    }
  }
  else if (transport_.WaitingForUnload()) {
    printf("Zoo no. %d got transport of %d ships of snow! Releasing.\n", rank_, snow_manager_.RequiredShips());

    Release(snow_manager_.RequiredShips(), ResourceType::kShip);
    Release(snow_manager_.RequiredPorts(), ResourceType::kPort);
    transport_.SetIdle();
  }
}

void Zoo::Request(int quantity, ResourceType type) {
  if (!snow_manager_.NeedNow()) return;

  int timestamp = time(nullptr);
  Message msg(communication_.Rank(), timestamp, quantity, type);
  communication_.SendAll(msg, Tag::kRequest);

  snow_manager_.Requested();
  resource_timestamp_[type] = timestamp;
}

void Zoo::Release(int quantity, ResourceType type) {
  Message release(rank_, time(nullptr), quantity, type);
  communication_.SendAll(release, Tag::kRelease);

  resource_amount_[type] += quantity;

  snow_manager_.Received();
}


}
