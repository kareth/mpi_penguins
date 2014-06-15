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
    usleep(100000);  // TODO unnecessary with introduction of random delays
    ProcessChanges();
    ProceedWithTransport();    // Once we got access to ships
    ReplyToRequests();
  }
}

void Zoo::ProcessChanges() {
  for (int i = 0; i < communication_.Size(); i++) {
    if (i != rank_ && communication_.Test(i, Tag::kRelease)) {
      ships_ += requests_[i].field(Field::kQuantity);
      communication_.Receive(i, &requests_[i], Tag::kRelease);
    }
  }

  for (int i = 0; i < communication_.Size(); i++) {
    if (i != rank_ && communication_.Test(i, Tag::kAcquire)) {
      ships_ -= requests_[i].field(Field::kQuantity);
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
  if (ship_request_timestamp_ > message.field(Field::kTimestamp)) return true;
  if (ship_request_timestamp_ < message.field(Field::kTimestamp)) return false;
  return rank_ < message.field(Field::kRank);
}

void Zoo::ProceedWithTransport() {
  if (transport_.Idle()) {
    printf("Zoo no. %d are requesting %d ships!\n", rank_, snow_manager_.RequiredShips());
    RequestResources();
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
      if (ships_ < snow_manager_.RequiredShips())
        return;
      // TODO check if ships <= MaxShips (noobish deadlock possibility)

      printf("Zoo no. %d acquired %d ships!\n", rank_, snow_manager_.RequiredShips());
      Message acquire(rank_, time(nullptr), snow_manager_.RequiredShips(), 0);
      communication_.SendAll(acquire, Tag::kAcquire);

      ships_ -= snow_manager_.RequiredShips();

      ReplyQueuedMessages();

      transport_.StartTransport();
    }
  }
  else if (transport_.WaitingForTransport()) {
    if (transport_.Arrived()) {
      ReleaseResources();

      // Move both lines to after unload.
      printf("Zoo no. %d got transport of %d ships of snow! Releasing.\n", rank_, snow_manager_.RequiredShips());
      transport_.SetIdle();
    }
  }
  else if (transport_.WaitingForPorts()) {
    // AMAN
  }
  else if (transport_.WaitingForUnload()) {
    // AMAN
  }
}

// TODO add type and amount here. its shipps by far
void Zoo::RequestResources() {
  if (!snow_manager_.NeedNow()) return;

  int timestamp = time(nullptr);
  Message msg(communication_.Rank(), timestamp, snow_manager_.RequiredShips(), 0); // 0 - first resource type?
  communication_.SendAll(msg, Tag::kRequest);

  snow_manager_.Requested();
  ship_request_timestamp_ = timestamp;
}

// TODO add type and amount here. its shipps by far
void Zoo::ReleaseResources() {
  Message release(rank_, time(nullptr), snow_manager_.RequiredShips(), 0);
  communication_.SendAll(release, Tag::kRelease);

  ships_ += snow_manager_.RequiredShips();

  snow_manager_.Received();
}


}
