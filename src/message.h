#ifndef MESSAGE_H_
#define MESSAGE_H_

namespace penguins {

enum class Field {
  kRank,
  kTimestamp
};

class Message {
 public:
  Message();
  Message(int rank, int timestamp);

  int* data() { return data_; }
  int field(const Field& field) const { return data_[int(field)]; }

  int length() const { return kLength; }

 private:
  static const int kLength = 4;

  int data_[kLength];
};

}  // namespace penguins

#endif  // MESSAGE_H_
