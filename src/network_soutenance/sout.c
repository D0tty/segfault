#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "../util/debug.h"
#include "../nn/network.h"
#include "test_network.h"

void test_net(network *nt)
{
	test_network_output(nt, (double[]){ 0, 0 });
  test_network_output(nt, (double[]){ 1, 0 });
  test_network_output(nt, (double[]){ 0, 1 });
  test_network_output(nt, (double[]){ 1, 1 });
}

int main()
{
	srand(time(NULL)); // Randomize the seed.

	char sel = '\0';
  char *file = NULL;
	network *nt = NULL;

  do
	{
		system("clear");
		printf("Select one choice:\n\n\t1: XOR\n\t2: AND\n\t3: OR\n\t4: XOR RANDOM\
        \n\n\t0: Quit\n\
				\nchoice:");
		fflush(stdin);
		scanf(" %c", &sel);
	}while( (sel != '1') && (sel != '2') && (sel != '3') && (sel != '4') &&\
					(sel != '0'));

  switch(sel)
	{
		case '0':
			return 0;

		case '1':
      file = "xor_trained.txt";
	    nt = load_network(file);
	    printf("The network have been created from this file: %s\n", file);
			test_net(nt);
			break;

		case '2':
      file = "and_trained.txt";
    	nt = load_network(file);
    	printf("The network have been created from this file: %s\n", file);
			test_net(nt);
			break;

		case '3':
      file = "or_trained.txt";
    	nt = load_network(file);
	    printf("The network have been created from this file: %s\n", file);
			test_net(nt);
			break;

    case '4':
     test_network_sgd();
     break;

		default :
			return 2;
	}

	return 0;
}
