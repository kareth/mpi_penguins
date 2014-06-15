#include "zoo.h"

#include <cstdio>

namespace penguins {

void Zoo::Run() {
  printf("Hello!\n");
  printf("Rank: %d\n", communication_.Rank());
  printf("Size: %d\n", communication_.Size());
}

}
