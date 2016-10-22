#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "network.h"
#include "../util/vector.h"
#include "../util/debug.h"

int main(/*int argc, char *argv[]*/)
{
	srand(time(NULL)); // Randomize the seed.

  size_t sizes[] = { 2, 3, 1 };
	network* nt = create_network(sizes, 3);
  
  print_network(nt, 1);

  save_network(nt,"save.txt");
  free_network(nt);

  nt = load_network("save.txt");
  save_network(nt,"save_after_laod.txt");
  free_network(nt);
  
  return 0;
}
