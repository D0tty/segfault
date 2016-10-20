#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "network.h"
#include "../util/vector.h"

int main(/*int argc, char *argv[]*/)
{
	srand(time(NULL)); // Randomize the seed.

  double mat[] = {
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
  };

  double* output = malloc(9 * sizeof (double));

  print_vector(mat, 3, 3);
  printf("\n");

  vector_substract(output, mat, mat, 9);
  print_list(output, 9);
  printf("\n");
  //
  // size_t sizes[] = { 2, 2 };
	// network* nt = create_network(sizes, 2);
  // print_network(nt, 1);
  // double input[] = { 0.3, 0.6 };
  // double* activations = feedforward(nt, input);
  // print_list(activations, 2);
  // free_network(nt);
  return 0;
}
