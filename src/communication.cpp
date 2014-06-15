#include "communication.h"

namespace penguins {


void Communication::Receive(int source, Message* msg, const Tag& tag) {
  MPI::COMM_WORLD.Iprobe(source, int(tag));

  MPI::COMM_WORLD.Irecv(
    msg->data(), msg->length(), MPI_INT, source, int(tag));
}

void Communication::ReceiveAll(Message* msgs, const Tag& tag) {
  for (int i = 0; i < Size(); i++)
    if (i != Rank())
      Receive(i, &msgs[i], tag);
}

// MPI::Send doesnt guarantee constnessf
void Communication::Send(int dest, Message& msg, const Tag& tag) {
  MPI::COMM_WORLD.Send(
    msg.data(), msg.length(), MPI_INT, dest, int(tag));
}

// MPI::Send doesnt guarantee constness
void Communication::SendAll(Message& msg, const Tag& tag) {
  for (int i = 0; i < Size(); i++)
    if (i != Rank())
      Send(i, msg, tag);
}

bool Communication::Test(int source, const Tag& tag) {
  return Request(source, tag)->Test();
}

bool Communication::TestAll(const Tag& tag) {
  for (int i = 0; i < Size(); i++)
    if (i != Rank() && !Request(i, tag)->Test())
        return false;
  return true;
}

int Communication::Rank() {
  return MPI::COMM_WORLD.Get_rank();
}

int Communication::Size() {
  return MPI::COMM_WORLD.Get_size();
}

}  // namespace penguins
