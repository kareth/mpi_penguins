#include <stdio.h>

#include "mpi.h"

#include "zoo.h"

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  penguins::Zoo zoo;
  zoo.Run();

  printf("Finalize\n");
  MPI_Finalize();
  return 0;
}
