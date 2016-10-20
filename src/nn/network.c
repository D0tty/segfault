#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../util/gaussrand.h"
#include "../util/vector.h"
#include "../util/debug.h"
#include "network.h"

// `sizes` is a list containing the respective sizes of the neuron layers of our
// network.
network* create_network(size_t* sizes, size_t nb_layers)
{
  network *nt;
  nt = malloc(sizeof (network));
  nt->nb_layers = nb_layers;
  nt->sizes = sizes;

  // The first layer doesn't have biases.
  size_t biases_length = nb_layers - 1;
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
  size_t weights_length = nb_layers - 1;
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
  for (size_t i = 0; i < nt->nb_layers - 1; i++)
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
  for (size_t i = 0; i < nt->nb_layers; ++i)
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

      if (i < nt->nb_layers - 1)
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
double sigmoid(double z)
{
  return 1. / (1. + exp(-z));
}

// Derivative of sigmoid.
double sigmoid_prime(double z)
{
  return sigmoid(z) * (1. - sigmoid(z));
}

void feedforward_step(network* nt, double* input, size_t layer_idx,
                      double* activations)
{
  size_t curr_size = nt->sizes[layer_idx]; // Current layer size
  size_t next_size = nt->sizes[layer_idx + 1]; // Next layer size
  double* next_biases = nt->biases[layer_idx]; // Next layer biases
  double* weights = nt->weights[layer_idx]; // Weights between current and next layer

  for (size_t j = 0; j < next_size; ++j)
  {
    double activation = next_biases[j];
    for (size_t k = 0; k < curr_size; ++k)
    {
      activation += weights[j * curr_size + k] * input[k];
    }
    activations[j] = activation;
  }
}

void feedforward(network* nt, double* input, double* activations)
{
  double* curr_activations = input;
  for (size_t i = 0; i < nt->nb_layers - 1; i++)
  {
    double* next_activations;
    if (i == nt->nb_layers - 2)
    {
      next_activations = activations;
    }
    else
    {
      next_activations = malloc(nt->sizes[i + 1] * sizeof (double));
    }
    feedforward_step(nt, curr_activations, i, next_activations);
    vector_apply(next_activations, &sigmoid, next_activations, nt->sizes[i + 1]);

    if (i > 0) {
      // We don't want to free the input, it doesn't belong to us.
      free(curr_activations);
    }

    curr_activations = next_activations;
  }
}

gradients* create_gradients(network* nt)
{
  size_t length = nt->nb_layers - 1;
  double** biases = malloc(length * sizeof (double*));
  double** weights = malloc(length * sizeof (double*));
  for (size_t i = 0; i < length; ++i)
  {
    biases[i] = malloc(nt->sizes[i] * sizeof (double));
    weights[i] = malloc(nt->sizes[i] * nt->sizes[i + 1] * sizeof (double));
  }
  gradients* grad = malloc(sizeof (gradients));
  grad->biases = biases;
  grad->weights = weights;
  return grad;
}

void free_gradients(network* nt, gradients* grad)
{
  size_t length = nt->nb_layers - 1;
  for (size_t i = 0; i < length; ++i)
  {
    free(grad->biases[i]);
    free(grad->weights[i]);
  }
  free(grad->biases);
  free(grad->weights);
}

void backprop(network* nt, training_datum* td, gradients* grad)
{
  size_t nb_layers = nt->nb_layers;
  size_t nb_inter = nt->nb_layers - 1;
  double** activations_list = malloc(nb_layers * sizeof (double*));
  double** activations_prime_list = malloc(nb_inter * sizeof (double*));
  activations_list[0] = td->input;
  for (size_t i = 0; i < nb_inter; ++i)
  {
    // @TODO: Free the activations allocations
    size_t activations_length = nt->sizes[i + 1];
    size_t activations_size = activations_length * sizeof (double);
    double* activations = malloc(activations_size);
    double* activations_prime = malloc(activations_size);
    feedforward_step(nt, activations_list[i], i, activations);
    memcpy(activations_prime, activations, activations_size);
    vector_apply(activations, sigmoid, activations, activations_length);
    vector_apply(activations_prime, sigmoid_prime, activations_prime, activations_length);
    activations_list[i + 1] = activations;
    activations_prime_list[i] = activations_prime;
  }

  double* delta = grad->biases[nb_inter - 1];
  size_t layer_size = nt->sizes[nb_layers - 1];
  memcpy(delta, activations_list[nb_layers - 1], layer_size * sizeof (double));
  vector_substract(delta, delta, td->output, layer_size); // Cost derivative
  vector_multiply(delta, delta, activations_prime_list[nb_inter - 1], layer_size);
  dot_it(grad->weights[nb_inter - 1], delta, activations_list[nb_layers - 2], layer_size, 1, layer_size);

  for (size_t i = 2; i < nb_layers; ++i)
  {
    double* activations_prime = activations_prime_list[nb_inter - i];
    double* next_delta = grad->biases[nb_inter - i];
    dot_ti(next_delta, nt->weights[nb_inter - i + 1], delta, nt->sizes[nb_inter - i + 1], nt->sizes[nb_inter - i + 2], 1);
    vector_multiply(next_delta, next_delta, activations_prime, nt->sizes[nb_inter - i + 1]);
    // Not sure at all about the dimensions here!
    dot_it(grad->weights[nb_inter - i], next_delta, activations_list[nb_layers - i - 1], nt->sizes[nb_inter - i], 1, nt->sizes[nb_inter - i + 1]);
    delta = next_delta;
  }
}

// void add_gradients(network* nt, gradients* a, gradients* b)
// {
//   size_t length = nt->nb_layers - 1;
//   for (size_t i = 0; i < bias_weight_size; ++i)
//   {
//     vector_add(a->biases[i], a->biases[i], b->biases[i], nt->sizes[i]);
//     vector_add(a->weights[i], a->weights[i], b->weights[i], nt->sizes[i] * nt->sizes[i + 1]);
//   }
// }
//
// void train(network* nt, training_datum** training_data,
//            size_t training_data_length, double eta)
// {
//   gradients* grad = create_gradients(nt);
//
//   for (size_t i = 0; i < training_data_length; ++i)
//   {
//     training_datum* td = training_data[i];
//     gradients* delta = backprop(nt, td);
//     add_gradients(grad, delta);
//     free_gradients(delta);
//   }
//
//   double batch_factor = -eta / (double)training_data_length;
//   for (size_t i = 0; i < bias_weight_size; ++i)
//   {
//     for (size_t j = 0; j < nt->sizes[i]; ++j)
//     {
//       nt->biases[i][j] += batch_factor * grad->biases[i][j];
//     }
//     for (size_t j = 0; j < nt->sizes[i] * nt->sizes[i + 1]; ++j)
//     {
//       nt->weights[i][j] += batch_factor * grad->weights[i][j];
//     }
//   }
//
//   free_gradients(grad);
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
