#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <err.h>
#include "../util/gaussrand.h"
#include "../util/vector.h"
#include "../util/debug.h"
#include "../util/shuffle.h"
#include "../util/misc.h"
#include "network.h"

// `sizes` is a list containing the respective sizes of the neuron layers of our
// network.
network* create_network(size_t* sizes, size_t nb_layers)
{
  network *nt;
  nt = malloc(sizeof (network));
  nt->nb_layers = nb_layers;
  nt->sizes = sizes;

  // Beware! The first layer doesn't have biases. As such, the biases sizes
  // at index 1 in the sizes list.
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

// Execute one neuron layer on input.
// This is useful for the backpropagation algorithm, where we want to store the
// activations at every layer of the network in order to estimate the effect
// of the backpropagation.
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
    // ∑(Weight_k * Input_k) + Bias
    activations[j] = activation;
  }
}

// Execute the neural network for a given `input` of size `nt->sizes[0]`.
// Output goes into `activations`, which should have size `nt->sizes[-1]`.
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
      // @TODO: Potential optimization, allocate only one array of size
      // `max(nt->sizes)`.
      next_activations = malloc(nt->sizes[i + 1] * sizeof (double));
    }
    feedforward_step(nt, curr_activations, i, next_activations);
    // `feedforward_step` doesn't apply `sigmoid` directly because we sometimes
    // want to apply `sigmoid_prime` instead (see `backprop`).
    // @TODO: Potential optimization, create two different `feedforward_step`,
    // one that only applies `sigmoid` with one output, and one that applies
    // `sigmoid` and `sigmoid_prime` in the same loop, with two outputs.
    // Note: this would remove the need for `vector_apply`.
    vector_apply(next_activations, &sigmoid, next_activations, nt->sizes[i + 1]);

    if (i > 0) {
      // We don't want to free the first input, it doesn't belong to us.
      free(curr_activations);
    }

    curr_activations = next_activations;
  }
}

// Create gradients with empty weights and biases.
gradients* create_gradients(network* nt)
{
  size_t nb_inter = nt->nb_layers - 1;
  double** biases = malloc(nb_inter * sizeof (double*));
  double** weights = malloc(nb_inter * sizeof (double*));
  for (size_t i = 0; i < nb_inter; ++i)
  {
    biases[i] = malloc(nt->sizes[i + 1] * sizeof (double));
    weights[i] = malloc(nt->sizes[i] * nt->sizes[i + 1] * sizeof (double));

    for (size_t j = 0; j < nt->sizes[i + 1]; ++j)
    {
      biases[i][j] = 0;
    }
    for (size_t j = 0; j < nt->sizes[i] * nt->sizes[i + 1]; ++j)
    {
      weights[i][j] = 0;
    }
  }
  gradients* grad = malloc(sizeof (gradients));
  grad->biases = biases;
  grad->weights = weights;
  return grad;
}

// Free the provided gradients.
void free_gradients(network* nt, gradients* grad)
{
  size_t nb_inter = nt->nb_layers - 1;
  for (size_t i = 0; i < nb_inter; ++i)
  {
    free(grad->biases[i]);
    free(grad->weights[i]);
  }
  free(grad->biases);
  free(grad->weights);
  free(grad);
}

// Print the gradients.
void print_grad(network* nt, gradients* grad)
{
  printf("biases: [\n");
  for (size_t i = 0; i < nt->nb_layers - 1; ++i)
  {
    printf("  ");
    print_list(grad->biases[i], nt->sizes[i + 1]);
    printf(",\n");
  }
  printf("]\n");
  printf("weights: [\n");
  for (size_t i = 0; i < nt->nb_layers - 1; ++i)
  {
    printf("  ");
    print_list(grad->weights[i], nt->sizes[i] * nt->sizes[i + 1]);
    printf(",\n");
  }
  printf("]\n\n");
}

// Backpropagates a change to the final output through all layers in order to
// figure out a gradient that minimizes the cost for a given training data.
void backprop(network* nt, training_datum* td, gradients* grad)
{
  size_t nb_layers = nt->nb_layers;
  size_t nb_inter = nt->nb_layers - 1;

  // Compute the list of all activations for every layer, as well as the list
  // of activations through sigmoid prime (which tells us in which direction to
  // go to increase/decrease the activation).
  double** activations_list = malloc(nb_layers * sizeof (double*));
  double** activations_prime_list = malloc(nb_inter * sizeof (double*));
  activations_list[0] = td->input;
  for (size_t i = 0; i < nb_inter; ++i)
  {
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
  dot_ti(grad->weights[nb_inter - 1], delta, activations_list[nb_layers - 2], layer_size, 1, nt->sizes[nb_layers - 2]);
  // Δ = (acti[-1] - output) * acti'[-1]
  // ∇biases[-1] = Δ
  // ∇weights[-1] = dot(Δ, transpose(acti[-2]))
  // In our case, we dot(transpose(Δ), acti[-2]) since our matrices are already
  // transposed in memory.
  // Which is strange, since we don't need to do it in the loop below.
  // @TODO: Investigate.

  for (size_t i = 2; i < nb_layers; ++i)
  {
    double* activations_prime = activations_prime_list[nb_inter - i];
    double* next_delta = grad->biases[nb_inter - i];
    dot_ti(next_delta, nt->weights[nb_inter - i + 1], delta, nt->sizes[nb_inter - i + 1], nt->sizes[nb_inter - i + 2], 1);
    vector_multiply(next_delta, next_delta, activations_prime, nt->sizes[nb_inter - i + 1]);
    dot_it(grad->weights[nb_inter - i], next_delta, activations_list[nb_layers - i - 1], nt->sizes[nb_inter - i + 1], 1, nt->sizes[nb_inter - i]);
    delta = next_delta;
    // Δ = dot(transpose(weights[-i + 1]), Δ) * acti'[-i]
    // ∇biases[-i] = Δ
    // ∇weights[-i] = dot(Δ, transpose(acti[-i - 1]))
  }

  for (size_t i = 0; i < nb_inter; ++i)
  {
    // The first activation is the input, which was passed as argument, so
    // we don't free it since it doesn't belong to us.
    free(activations_list[i + 1]);
    free(activations_prime_list[i]);
  }
  free(activations_list);
  free(activations_prime_list);
}

// Add two gradients.
void add_gradients(network* nt, gradients* a, gradients* b)
{
  size_t nb_inter = nt->nb_layers - 1;
  for (size_t i = 0; i < nb_inter; ++i)
  {
    vector_add(a->biases[i], a->biases[i], b->biases[i], nt->sizes[i + 1]);
    vector_add(a->weights[i], a->weights[i], b->weights[i], nt->sizes[i] * nt->sizes[i + 1]);
  }
}

// Train the network on the provided training data with the provided learning
// rate.
void train(network* nt, training_datum** training_data,
    size_t training_data_length, double learning_rate)
{
  gradients* grad = create_gradients(nt);

  for (size_t i = 0; i < training_data_length; ++i)
  {
    training_datum* td = training_data[i];
    gradients* delta = create_gradients(nt);
    backprop(nt, td, delta);
    add_gradients(nt, grad, delta);
    free_gradients(nt, delta);
  }

  // `grad` now contains the changes we need to apply to our network's biases
  // and weights in order to minimize the cost for our training data.

  size_t nb_inter = nt->nb_layers - 1;
  double batch_factor = -learning_rate / (double)training_data_length;
  for (size_t i = 0; i < nb_inter; ++i)
  {
    // Optimization: could use `vector_scalar_multiply` and `vector_add` here,
    // but it would imply two more loops.
    for (size_t j = 0; j < nt->sizes[i + 1]; ++j)
    {
      nt->biases[i][j] += batch_factor * grad->biases[i][j];
    }
    for (size_t j = 0; j < nt->sizes[i] * nt->sizes[i + 1]; ++j)
    {
      nt->weights[i][j] += batch_factor * grad->weights[i][j];
    }
  }

  free_gradients(nt, grad);
}

// Stochastic gradient descent.
void sgd(network* nt, training_datum** training_data,
    size_t training_data_length, unsigned epochs, size_t mini_batch_size,
    double eta)
{
  for (size_t epoch = 0; epoch < epochs; epoch++)
  {
    training_datum** td = malloc(training_data_length * sizeof (training_datum*));
    shuffle(td, training_data, training_data_length, sizeof (training_datum*));
    for (size_t i = 0; i < training_data_length; i += mini_batch_size)
    {
      train(nt, td + i, MIN(mini_batch_size, training_data_length - i), eta);
    }
    free(td);
  }
}

void save_network(network* nt, char* file)
{
  FILE *f = fopen(file, "w");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  //nt->nb_layers = nb_layers;
  fprintf(f, "%lu\n", nt->nb_layers);
  //nt->sizes = sizes;
  for( size_t i = 0; i < nt->nb_layers; ++i)
  {
    fprintf(f, "%lu\n", nt->sizes[i]);
  }

  // Beware! The first layer doesn't have biases. As such, the biases sizes
  // at index 1 in the sizes list.
  size_t biases_length = nt->nb_layers - 1;
  //double** biases = malloc(biases_length * sizeof (double*));
  for (size_t i = 0; i < biases_length; i++)
  {
    size_t size = nt->sizes[i + 1];
    //biases[i] = malloc(size * sizeof (double));
    for (size_t j = 0; j < size; j++)
    {
      fprintf(f, "%f\n", nt->biases[i][j]);
    }
  }
  //nt->biases = biases;

  // Wijk is the weight between the kth neuron in the ith layer and the jth
  // neuron in the (i+1)th layer.
  size_t weights_length = nt->nb_layers - 1;
  //double** weights = malloc(weights_length * sizeof (double*));
  for (size_t i = 0; i < weights_length; i++)
  {
    size_t next_size = nt->sizes[i + 1]; // Next layer size
    size_t curr_size = nt->sizes[i]; // Current layer size
    //weights[i] = malloc(curr_size * next_size * sizeof (double));
    // Why use next_layer index before curr_layer?
    // See http://neuralnetworksanddeeplearning.com/chap1.html#mjx-eqn-22
    for (size_t j = 0; j < next_size; j++)
    {
      for (size_t k = 0; k < curr_size; k++)
      {
        //weights[i][j * curr_size + k] = gaussrand();
        fprintf(f, "%f\n", nt->weights[i][j * curr_size + k]);
      }
    }
  }
  //nt->weights = weights;
  fclose(f);
}

network* load_network(char *file)
{
  char *f_line = NULL;
  size_t f_line_len = 0;
  size_t f_size_t_val = 0;

  FILE *f = fopen(file, "r");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  network *nt;
  nt = malloc(sizeof (network));

  //get nb_layers
  getline(&f_line, &f_line_len, f);
  sscanf(f_line, "%zu", &f_size_t_val);
  nt->nb_layers = f_size_t_val;
  size_t nb_layers = nt->nb_layers;

  nt->sizes = malloc( nt->nb_layers * sizeof(size_t) );
  for( size_t i = 0; i < nb_layers; ++i)
  {
    getline(&f_line, &f_line_len, f);
    sscanf(f_line, "%zu", &f_size_t_val);
    nt->sizes[i] = f_size_t_val;
  }

  // Beware! The first layer doesn't have biases. As such, the biases sizes
  // at index 1 in the sizes list.
  size_t biases_length = nb_layers - 1;
  double** biases = malloc(biases_length * sizeof (double*));
  for (size_t i = 0; i < biases_length; i++)
  {
    size_t size = nt->sizes[i + 1];
    biases[i] = malloc(size * sizeof (double));
    for (size_t j = 0; j < size; j++)
    {
      //biases[i][j] = gaussrand();
      getline(&f_line, &f_line_len, f);
      biases[i][j] = strtod(f_line,NULL);
    }
  }
  nt->biases = biases;

  // Wijk is the weight between the kth neuron in the ith layer and the jth
  // neuron in the (i+1)th layer.
  size_t weights_length = nb_layers - 1;
  double** weights = malloc(weights_length * sizeof (double*));
  for (size_t i = 0; i < weights_length; i++)
  {
    size_t curr_size = nt->sizes[i]; // Current layer size
    size_t next_size = nt->sizes[i + 1]; // Next layer size
    weights[i] = malloc(curr_size * next_size * sizeof (double));
    // Why use next_layer index before curr_layer?
    // See http://neuralnetworksanddeeplearning.com/chap1.html#mjx-eqn-22
    for (size_t j = 0; j < next_size; j++)
    {
      for (size_t k = 0; k < curr_size; k++)
      {
        //weights[i][j * curr_size + k] = gaussrand();
        getline(&f_line, &f_line_len, f);
        weights[i][j * curr_size + k] = strtod(f_line,NULL);
      }
    }
  }
  nt->weights = weights;

  fclose(f);

  return nt;
}
