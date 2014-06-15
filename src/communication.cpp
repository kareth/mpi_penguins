#include "communication.h"

namespace penguins {

int Communication::Rank() {
  return MPI::COMM_WORLD.Get_rank();
}

int Communication::Size() {
  return MPI::COMM_WORLD.Get_size();
}

}  // namespace penguins
