#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "../nn/network.h"
#include "../util/debug.h"

// Somewhat equal.
int seq(double a, double b)
{
  double diff = a - b;
  return diff < 0.00001 && diff > -0.00001;
}

void test_network_feedforward()
{
  size_t sizes[] = { 2, 2 };
	network* nt = create_network(sizes, 2);
  double** w = nt->weights;
  double** b = nt->biases;
  double input[] = { 0.3, 0.6 };
  double* activations = malloc(sizes[1] * sizeof (double));
  feedforward(nt, input, activations);
  double ff_0_0 = sigmoid(w[0][0] * input[0] + w[0][1] * input[1] + b[0][0]);
  double ff_0_1 = sigmoid(w[0][2] * input[0] + w[0][3] * input[1] + b[0][1]);
  // @TODO: investigate why we lose precision here.
  assert(seq(activations[0], ff_0_0) && seq(activations[1], ff_0_1));
  free(activations);
  free_network(nt);

  printf("feedforward: ok\n");
}

training_datum* create_training_datum(double* input, double* output)
{
  training_datum* td = malloc(sizeof (training_datum));
  td->input = input;
  td->output = output;
  return td;
}

void test_network_output(network* nt, double* input)
{
  double* activations = malloc(nt->sizes[nt->nb_layers - 1] * sizeof (double));
  feedforward(nt, input, activations);
  print_list(input, nt->sizes[0]);
  printf(": ");
  print_list(activations, nt->sizes[nt->nb_layers - 1]);
  printf("\n");
}

void test_network_sgd()
{
  size_t sizes[] = { 2, 3, 1 };
	network* nt = create_network(sizes, 3);

  training_datum* datum1 = create_training_datum(
    (double[]){ 0, 0 },
    (double[]){ 0 }
  );
  training_datum* datum2 = create_training_datum(
    (double[]){ 1, 0 },
    (double[]){ 1 }
  );
  training_datum* datum3 = create_training_datum(
    (double[]){ 0, 1 },
    (double[]){ 1 }
  );
  training_datum* datum4 = create_training_datum(
    (double[]){ 1, 1 },
    (double[]){ 0 }
  );
  training_datum* data[] = { datum1, datum2, datum3, datum4 };

  sgd(nt, data, 4, 100000000, 4, 10.);

  test_network_output(nt, (double[]){ 0, 0 });
  test_network_output(nt, (double[]){ 1, 0 });
  test_network_output(nt, (double[]){ 0, 1 });
  test_network_output(nt, (double[]){ 1, 1 });

  free_network(nt);
  free(datum1);
  free(datum2);
  free(datum3);
  free(datum4);

  printf("sgd: ok\n");
}

void test_network()
{
	srand(time(NULL)); // Randomize the seed.

  test_network_feedforward();
  test_network_sgd();
}
