#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../util/gaussrand.h"
#include "network.h"

// `sizes` is a list containing the respective sizes of the neuron layers of our
// network.
network* create_network(size_t* sizes, size_t sizes_length)
{
  network *nt;
  nt = malloc(sizeof (network));
  nt->sizes_length = sizes_length;
  nt->sizes = sizes;

  // The first layer doesn't have biases.
  size_t biases_length = sizes_length - 1;
  double** biases = malloc(biases_length * sizeof (double*));
  for (size_t i = 0; i < biases_length; i++)
  {
    size_t size = sizes[i + 1];
    biases[i] = malloc(size * sizeof (double));
    for (size_t j = 0; j < size; j++)
    {
      biases[i][j] = gaussrand();
    }
  }
  nt->biases = biases;

  // Wijk is the weight between the kth neuron in the ith layer and the jth
  // neuron in the (i+1)th layer.
  size_t weights_length = sizes_length - 1;
  double** weights = malloc(weights_length * sizeof (double*));
  for (size_t i = 0; i < weights_length; i++)
  {
    size_t curr_size = sizes[i]; // Current layer size
    size_t next_size = sizes[i + 1]; // Next layer size
    weights[i] = malloc(curr_size * next_size * sizeof (double));
    // Why use next_layer index before curr_layer?
    // See http://neuralnetworksanddeeplearning.com/chap1.html#mjx-eqn-22
    for (size_t j = 0; j < next_size; j++)
    {
      for (size_t k = 0; k < curr_size; k++)
      {
        weights[i][j * curr_size + k] = gaussrand();
      }
    }
  }
  nt->weights = weights;

  return nt;
}

void free_network(network* nt)
{
  for (size_t i = 0; i < nt->sizes_length - 1; i++)
  {
    free(nt->biases[i]);
    free(nt->weights[i]);
  }
  free(nt->biases);
  free(nt->weights);
  free(nt);
}

void print_network(network* nt, int with_edges)
{
  printf("digraph Network {\n");
  printf("  rankdir=LR;\n"); // Left to right
  printf("  splines=false;\n"); // Force straight lines, despite labels
  for (size_t i = 0; i < nt->sizes_length; ++i)
  {
    for (size_t j = 0; j < nt->sizes[i]; ++j)
    {
      if (i > 0)
      {
        printf("  n_%zu_%zu [label=\"%f\"];\n", i, j, nt->biases[i - 1][j]);
      }
      else
      {
        printf("  n_%zu_%zu [label=\"%zu\"];\n", i, j, j);
      }

      if (i < nt->sizes_length - 1)
      {
        for (size_t k = 0; k < nt->sizes[i + 1]; ++k)
        {
          if (with_edges)
          {
            printf("  n_%zu_%zu_%zu_%zu [label=\"%f\", shape=box, fontsize=10, margin=0];\n",
                   i, j, i + 1, k, nt->weights[i][k * nt->sizes[i] + j]);
            printf("  n_%zu_%zu -> n_%zu_%zu_%zu_%zu [arrowhead=none];\n", i, j,
                   i, j, i + 1, k);
            printf("  n_%zu_%zu_%zu_%zu -> n_%zu_%zu;\n", i, j, i + 1, k, i + 1,
                   k);
          }
          else
          {
            printf("  n_%zu_%zu -> n_%zu_%zu;\n", i, j, i + 1, k);
          }
        }
      }
    }
  }
  printf("}\n");
}


// Our activation function.
static inline
double sigmoid(double z)
{
  return 1. / (1. + exp(-z));
}

double* feedforward(network* nt, double* input)
{
  double* activations = input;
  for (size_t i = 0; i < nt->sizes_length - 1; i++)
  {
    size_t curr_size = nt->sizes[i]; // Current layer size
    size_t next_size = nt->sizes[i + 1]; // Next layer size
    double* next_biases = nt->biases[i]; // Next layer biases
    double* weights = nt->weights[i]; // Weights between current and next layer

    double* next_activations = malloc(next_size * sizeof (double));
    for (size_t j = 0; j < next_size; ++j)
    {
      double activation = 0;
      for (size_t k = 0; k < curr_size; ++k)
      {
        activation += weights[j * curr_size + k] * activations[k];
      }
      next_activations[j] = sigmoid(activation + next_biases[j]);
    }

    if (i > 0) {
      // We don't want to free the input, it doesn't belong to us.
      free(activations);
    }

    activations = next_activations;
  }

  return activations;
}

// void train(network* nt, training_datum** training_data,
//            size_t training_data_length, double eta)
// {
//   size_t bias_weight_size = nt->sizes_length - 1;
//   double** nabla_b = malloc(bias_weight_size * sizeof (double*));
//   double** nabla_w = malloc(bias_weight_size * sizeof (double*));
//   for (size_t i = 0; i < bias_weight_size; ++i)
//   {
//     nabla_b[i] = malloc(nt->sizes[i] * sizeof (double));
//     nabla_w[i] = malloc(nt->sizes[i] * nt->sizes[i + 1] * sizeof (double));
//   }
//
//   for (size_t i = 0; i < training_data_length; ++i)
//   {
//
//   }
// }
//
// // Stochastic gradient descent
// void sgd(network* nt, training_datum** training_data,
//          size_t training_data_length, unsigned epochs, size_t mini_batch_size,
//          double eta)
// {
//   for (size_t epoch = 0; epoch < epochs; epoch++)
//   {
//     training_datum** td = shuffle(training_data, training_data_length,
//                                   sizeof (training_datum*));
//     for (size_t i = 0; i < training_data_length; i += mini_batch_size)
//     {
//       train(nt, training_data + i,
//             min(mini_batch_size, training_data_length - i), eta);
//     }
//   }
// }
