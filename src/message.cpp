#include "message.h"

#include <algorithm>

namespace penguins {

Message::Message() {
  std::fill_n(data_, kLength, -1);
}


Message::Message(int rank, int type) : Message() {
  data_[int(Field::kRank)] = rank;
  data_[int(Field::kType)] = type;
}

Message::Message(int rank, int timestamp, int quantity, int type) {
  data_[int(Field::kRank)] = rank;
  data_[int(Field::kTimestamp)] = timestamp;
  data_[int(Field::kQuantity)] = quantity;
  data_[int(Field::kType)] = type;
}

}  // namespace penguins
