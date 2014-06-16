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
    if (i != communication.Rank() && communication.Test(i, Tag::kRelease) && requests_[int(Tag::kRelease)][i].field(Field::kType) == type_) {
      amount_ += requests_[int(Tag::kRelease)][i].field(Field::kQuantity);
      communication.Receive(i, &requests_[int(Tag::kRelease)][i], Tag::kRelease);
    }
  }

  for (int i = 0; i < communication.Size(); i++) {
    if (i != communication.Rank() && communication.Test(i, Tag::kAcquire) && requests_[int(Tag::kAcquire)][i].field(Field::kType) == type_) {
      /*printf("Rank: %d\n", communication.Rank());
      printf("%d ", requests_[int(Tag::kAcquire)][i].field(Field::kRank));
      printf("%d ", requests_[int(Tag::kAcquire)][i].field(Field::kTimestamp));
      printf("%d ", requests_[int(Tag::kAcquire)][i].field(Field::kQuantity));
      printf("%d\n\n",  requests_[int(Tag::kAcquire)][i].field(Field::kType));*/
      amount_ -= requests_[int(Tag::kAcquire)][i].field(Field::kQuantity);
      communication.Receive(i, &requests_[int(Tag::kAcquire)][i], Tag::kAcquire);
    }
  }
}

void Resource::ReplyToRequests(Communication& communication) {
  for (int i = 0; i < communication.Size(); i++) {
    if (i != communication.Rank() && communication.Test(i, Tag::kRequest) && requests_[int(Tag::kRequest)][i].field(Field::kType) == type_) {
      //printf("%d received request from %d\n", communication.Rank(), requests_[int(Tag::kRequest)][i].field(Field::kRank));
      if (ShouldWaitFor(requests_[int(Tag::kRequest)][i], communication)) {
        //printf("%d Sending ACC to %d\n", communication.Rank(), requests_[int(Tag::kRequest)][i].field(Field::kRank));
        Message reply(communication.Rank(), requests_[int(Tag::kRequest)][i].field(Field::kType));
        communication.Send(requests_[int(Tag::kRequest)][i].field(Field::kRank), reply, Tag::kReply);
      } else {
        //printf("%d has priority over %d\n", communication.Rank(), requests_[int(Tag::kRequest)][i].field(Field::kRank));
        queue_.insert(requests_[int(Tag::kRequest)][i]);
      }

      communication.Receive(i, &requests_[int(Tag::kRequest)][i], Tag::kRequest);
    }
  }
}

void Resource::ReplyQueuedMessages(Communication& communication) {
  while (queue_.size() > 0) {
    auto msg = *queue_.begin();
    queue_.erase(queue_.begin());

    Message reply(communication.Rank(), msg.field(Field::kType));
    communication.Send(msg.field(Field::kRank), reply, Tag::kReply);
    //printf("%d Sending ACC to %d\n", communication.Rank(), msg.field(Field::kRank));
  }
}

bool Resource::ShouldWaitFor(const Message& message, Communication& communication) {
  if (timestamp_ > message.field(Field::kTimestamp)) return true;
  if (timestamp_ < message.field(Field::kTimestamp)) return false;
  return communication.Rank() < message.field(Field::kRank);
}

}
