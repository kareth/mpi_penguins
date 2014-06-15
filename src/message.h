#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <algorithm>

namespace penguins {

enum class Field {
  kRank,
  kTimestamp,
  kQuantity,
  kType
};

class Message {
 public:
  Message();
  Message(int rank, int type);
  Message(int rank, int timestamp, int quantity, int type);

  int* data() { return data_; }
  int field(const Field& field) const { return data_[int(field)]; }

  int length() const { return kLength; }

  bool operator < (const Message& rhs) const {
    return std::make_pair(    field(Field::kTimestamp),     field(Field::kRank)) <
           std::make_pair(rhs.field(Field::kTimestamp), rhs.field(Field::kRank)); }

 private:
  static const int kLength = 4;

  int data_[kLength];
};

}  // namespace penguins

#endif  // MESSAGE_H_
