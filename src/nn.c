#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "nn.h"

// `sizes` is a list containing the respective sizes of the neuron layers of our
// network.
struct network* create_network(size_t* sizes, size_t sizes_length)
{
  struct network *nt;
  nt = malloc(sizeof (struct network));
  nt->sizes_length = sizes_length;
  nt->sizes = sizes;

  // The first layer doesn't have biases.
  size_t biases_length = sizes_length - 1;
  double** biases = malloc(biases_length * sizeof (double*));
  for (size_t i = 0; i < biases_length; i++)
  {
    size_t size = sizes[i];
    biases[i] = malloc(size * sizeof (double));
    for (size_t j = 0; j < size; j++)
    {
      biases[i][j] = 0.; // TODO: fill with normal distribution numbers
    }
  }
  nt->biases = biases;

  // Wijk is the weight between the kth neuron in the ith layer and the jth
  // neuron in the (i+1)th layer.
  size_t weights_length = sizes_length - 1;
  double** weights = malloc(weights_length * sizeof (double*));
  for (size_t i = 0; i < weights_length; i++)
  {
    size_t curr_layer_size = sizes[i];
    size_t next_layer_size = sizes[i + 1];
    weights[i] = malloc(curr_layer_size * next_layer_size * sizeof (double));
    // Why use next_layer index before curr_layer?
    // See http://neuralnetworksanddeeplearning.com/chap1.html#mjx-eqn-22
    for (size_t j = 0; j < next_layer_size; j++)
    {
      for (size_t k = 0; k < curr_layer_size; k++)
      {
        weights[i][j * curr_layer_size + k] = 0.; // TODO: fill with normal distribution numbers
      }
    }
  }
  nt->weights = weights;

  return nt;
}

// Our activation function.
// static inline
// double sigmoid(double z)
// {
//   return 1. / (1. + exp(-z));
// }
//
// // Warning: this function mutates the activations array passed as argument.
// void feedforward(struct network* nt, double* activations, size_t activations_length)
// {
//   for (size_t i = 0; i < nt->sizes_length - 1; i++)
//   {
//     double* biases = nt->biases[i];
//     double* weights = nt->weights[i];
//     activations = sigmoid(add(dot(weights, activations), b))
//   }
// }


int main(/*int argc, char *argv[]*/)
{
	printf("okaay\n");
  return 0;
}
