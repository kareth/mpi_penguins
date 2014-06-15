#include "message.h"

#include <algorithm>

namespace penguins {

Message::Message() {
  std::fill_n(data_, kLength, -1);
}

Message::Message(int rank, int timestamp) {
  data_[int(Field::kRank)] = rank;
  data_[int(Field::kTimestamp)] = timestamp;
}

}  // namespace penguins
