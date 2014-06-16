#include "zoo.h"

#include <cstdio>
#include <unistd.h>
#include <time.h>

namespace penguins {

void Zoo::Run() {
  rank_ = communication_.Rank();

  srand(rand() + rank_);

  communication_.ReceiveAll(port_.requests_, Tag::kRequest);
  communication_.ReceiveAll(ship_.requests_, Tag::kRequest);
  communication_.ReceiveAll(port_.requests_, Tag::kReply);
  communication_.ReceiveAll(ship_.requests_, Tag::kReply);
  MainLoop();
}

void Zoo::MainLoop() {
  while (true) {
    port_.ProcessChanges(communication_);
    ship_.ProcessChanges(communication_);

    ProceedWithTransport();    // Once we got access to ships

    ship_.ReplyToRequests(communication_);
    port_.ReplyToRequests(communication_);
  }
}

void Zoo::ProceedWithTransport() {
  if (transport_.Idle()) {
    snow_manager_.Next();
    printf("Zoo no. %d requests %d ship\n", rank_, snow_manager_.RequiredShips());
    ship_.Request(snow_manager_.RequiredShips(), communication_);

    transport_.WaitForShips();
  }
  else if (transport_.WaitingForShips()) {
    if (communication_.TestAll(Tag::kReply)) {
      ship_.ProcessChanges(communication_);

      // Important:
      // If there is not enough ships, just wait, they will release soon
      if (ship_.amount_ < snow_manager_.RequiredShips())
        return;
      // check for ships overflow
      if (ship_.amount_ > Configuration::MaxShips)
        return;

      printf("Zoo no. %d acquired %d ships!\n", rank_, snow_manager_.RequiredShips());
      Message acquire(rank_, time(nullptr), snow_manager_.RequiredShips(), ResourceType::kShip);
      communication_.SendAll(acquire, Tag::kAcquire);

      ship_.amount_ -= snow_manager_.RequiredShips();

      ship_.ReplyQueuedMessages(communication_);

      transport_.StartTransport();
    }
  }
  else if (transport_.WaitingForTransport()) {
    if (transport_.Arrived()) {
      printf("Zoo no. %d requests %d ports\n", rank_, snow_manager_.RequiredPorts());

      port_.Request(snow_manager_.RequiredPorts(), communication_);

      transport_.WaitForPorts();
    }
  }
  else if (transport_.WaitingForPorts()) {
    if (communication_.TestAll(Tag::kReply)) {
      port_.ProcessChanges(communication_);
      if (port_.amount_ < snow_manager_.RequiredPorts())
        return;
      if (port_.amount_ > Configuration::MaxPorts)
        return;

      printf("Zoo no. %d acquired %d ports!\n", rank_, snow_manager_.RequiredPorts());
      Message acquire(rank_, time(nullptr), snow_manager_.RequiredPorts(), ResourceType::kPort);
      communication_.SendAll(acquire, Tag::kAcquire);

      port_.amount_ -= snow_manager_.RequiredPorts();

      port_.ReplyQueuedMessages(communication_);

      transport_.StartUnload();
    }
  }
  else if (transport_.WaitingForUnload()) {
    if (transport_.Unloaded()) {
      printf("Zoo no. %d got transport of %d ships of snow! Releasing.\n", rank_, snow_manager_.RequiredShips());
      ship_.Release(snow_manager_.RequiredShips(), communication_);
      port_.Release(snow_manager_.RequiredPorts(), communication_);
      transport_.SetIdle();
    }
  }
}

}
