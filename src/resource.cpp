#include "resource.h"

namespace penguins {

void Resource::Request(int quantity, Communication& communication) {
  int timestamp = time(nullptr);
  Message request(communication.Rank(), timestamp, quantity, type_);
  communication.SendAll(request, Tag::kRequest);

  timestamp_ = timestamp;
}

void Resource::Release(int quantity, Communication& communication) {
  Message release(communication.Rank(), time(nullptr), quantity, type_);
  communication.SendAll(release, Tag::kRelease);

  amount_ += quantity;
}

void Resource::ProcessChanges(Communication& communication) {
  for (int i = 0; i < communication.Size(); i++) {
    if (i != communication.Rank() && communication.Test(i, Tag::kRelease) && requests_[i].field(Field::kType) == type_) {
      amount_ += requests_[i].field(Field::kQuantity);
      communication.Receive(i, &requests_[i], Tag::kRelease);
    }
  }

  for (int i = 0; i < communication.Size(); i++) {
    if (i != communication.Rank() && communication.Test(i, Tag::kAcquire) && requests_[i].field(Field::kType) == type_) {
      amount_ -= requests_[i].field(Field::kQuantity);
      communication.Receive(i, &requests_[i], Tag::kAcquire);
    }
  }
}

void Resource::ReplyToRequests(Communication& communication) {
  for (int i = 0; i < communication.Size(); i++) {
    if (i != communication.Rank() && communication.Test(i, Tag::kRequest)) {
      if (ShouldWaitFor(requests_[i], communication)) {
        Message reply(communication.Rank(), requests_[i].field(Field::kType));
        communication.Send(requests_[i].field(Field::kRank), reply, Tag::kReply);
      } else {
        queue_.insert(requests_[i]);
      }

      communication.Receive(i, &requests_[i], Tag::kRequest);
    }
  }
}

void Resource::ReplyQueuedMessages(Communication& communication) {
  while (queue_.size() > 0) {
    auto msg = *queue_.begin();
    queue_.erase(queue_.begin());

    Message reply(communication.Rank(), msg.field(Field::kType));
    communication.Send(msg.field(Field::kRank), reply, Tag::kReply);
  }
}

bool Resource::ShouldWaitFor(const Message& message, Communication& communication) {
  if (timestamp_ > message.field(Field::kTimestamp)) return true;
  if (timestamp_ < message.field(Field::kTimestamp)) return false;
  return communication.Rank() < message.field(Field::kRank);
}

}