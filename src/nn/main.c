#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "network.h"

void print_list(double* list, size_t length)
{
  printf("[");
  for (size_t i = 0; i < length; ++i)
  {
    printf("%f", list[i]);
    if (i != length - 1)
    {
      printf(", ");
    }
  }
  printf("]\n");
}

int main(/*int argc, char *argv[]*/)
{
	srand(time(NULL)); // Randomize the seed.

  size_t sizes[] = { 2, 2 };
	network* nt = create_network(sizes, 2);
  print_network(nt, 1);
  double input[] = { 0.3, 0.6 };
  double* activations = feedforward(nt, input);
  print_list(activations, 2);
  free_network(nt);
  return 0;
}
