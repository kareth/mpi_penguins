#include "zoo.h"

#include <cstdio>
#include <unistd.h>

namespace penguins {

void Zoo::Run() {
  printf("Hello!\n");
  printf("Rank: %d\n", communication_.Rank());
  printf("Size: %d\n", communication_.Size());


  Message msg(communication_.Rank(), 123);
  communication_.SendAll(msg, Tag::kRequest);

  usleep(1000000);  // 1s


  if (communication_.Rank() == 0) {
    Message msg(communication_.Rank(), 123);
    communication_.Send(1, msg, Tag::kRequest);

    usleep(1000000);  // 1s
  }

  if (communication_.Rank() == 1) {
    while (true) {
      usleep(1000000);  // 1s
      Message rec;
      communication_.Receive(0, &rec, Tag::kRequest);
      printf("%d process received message from %d\n",
          communication_.Rank(), rec.field(Field::kRank));
    }
  }
}

}
