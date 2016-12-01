#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <err.h>
#include <time.h>
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

  size_t nb_inter = nb_layers - 1;

  // No need to fill those lists with 0s, as they are always directly reassigned
  // with new values.
  double** activations_raw_list = malloc(nb_inter * sizeof (double*));
  double** activations_list = malloc(nb_layers * sizeof (double*));
  double** activations_prime_list = malloc(nb_inter * sizeof (double*));
  for (size_t i = 0; i < nb_inter; ++i)
  {
    size_t activations_length = nt->sizes[i + 1];
    size_t activations_size = activations_length * sizeof (double);
    double* activations_raw = malloc(activations_size);
    double* activations = malloc(activations_size);
    double* activations_prime = malloc(activations_size);
    // We ignore the first activation, as it is provided by the user.
    activations_raw_list[i] = activations_raw;
    activations_list[i + 1] = activations;
    activations_prime_list[i] = activations_prime;
  }
  nt->activations_raw_list = activations_raw_list;
  nt->activations_list = activations_list;
  nt->activations_prime_list = activations_prime_list;

  double** biases = malloc(nb_inter * sizeof (double*));
  double** biases_grad = malloc(nb_inter * sizeof (double*));
  double** biases_delta = malloc(nb_inter * sizeof (double*));
  for (size_t i = 0; i < nb_inter; i++)
  {
    // Beware! The first layer doesn't have biases. As such, the biases sizes
    // at index 1 in the sizes list.
    size_t size = sizes[i + 1];
    biases[i] = malloc(size * sizeof (double));
    biases_grad[i] = malloc(size * sizeof (double));
    biases_delta[i] = malloc(size * sizeof (double));
    for (size_t j = 0; j < size; j++)
    {
      biases[i][j] = gaussrand();
    }
  }
  nt->biases = biases;
  nt->biases_grad = biases_grad;
  nt->biases_delta = biases_delta;

  double** weights = malloc(nb_inter * sizeof (double*));
  double** weights_grad = malloc(nb_inter * sizeof (double*));
  double** weights_delta = malloc(nb_inter * sizeof (double*));
  for (size_t i = 0; i < nb_inter; i++)
  {
    // Wijk is the weight between the kth neuron in the ith layer and the jth
    // neuron in the (i+1)th layer.
    size_t curr_size = sizes[i]; // Current layer size
    size_t next_size = sizes[i + 1]; // Next layer size
    weights[i] = malloc(curr_size * next_size * sizeof (double));
    weights_grad[i] = malloc(curr_size * next_size * sizeof (double));
    weights_delta[i] = malloc(curr_size * next_size * sizeof (double));
  }
  nt->weights = weights;
  nt->weights_grad = weights_grad;
  nt->weights_delta = weights_delta;

  return nt;
}

void small_weights_init(network* nt)
{
  for (size_t i = 0; i < nt->nb_layers - 1; i++)
  {
    size_t curr_size = nt->sizes[i];
    size_t next_size = nt->sizes[i + 1];
    // Why use next_layer index before curr_layer?
    // See http://neuralnetworksanddeeplearning.com/chap1.html#mjx-eqn-22
    for (size_t j = 0; j < next_size; j++)
    {
      for (size_t k = 0; k < curr_size; k++)
      {
        // Small weight initialization
        nt->weights[i][j * curr_size + k] = gaussrand() / sqrt(curr_size);
      }
    }
  }
}

void free_network(network* nt)
{
  for (size_t i = 0; i < nt->nb_layers - 1; i++)
  {
    free(nt->activations_raw_list[i]);
    // The first activation is the input, which was passed as argument, so
    // we don't free it since it doesn't belong to us.
    free(nt->activations_list[i + 1]);
    free(nt->activations_prime_list[i]);
    free(nt->biases[i]);
    free(nt->biases_grad[i]);
    free(nt->biases_delta[i]);
    free(nt->weights[i]);
    free(nt->weights_grad[i]);
    free(nt->weights_delta[i]);
  }

  free(nt->activations_raw_list);
  free(nt->activations_list);
  free(nt->activations_prime_list);
  free(nt->biases);
  free(nt->biases_grad);
  free(nt->biases_delta);
  free(nt->weights);
  free(nt->weights_grad);
  free(nt->weights_delta);
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

// Print biases and weights
void print_biases_weights(network* nt, double** biases, double** weights)
{
  size_t nb_inter = nt->nb_layers - 1;
  printf("biases: [\n");
  for (size_t i = 0; i < nb_inter; ++i)
  {
    printf("  ");
    print_list(biases[i], nt->sizes[i + 1]);
    printf(",\n");
  }
  printf("]\n");
  printf("weights: [\n");
  for (size_t i = 0; i < nb_inter; ++i)
  {
    printf("  ");
    print_list(weights[i], nt->sizes[i] * nt->sizes[i + 1]);
    printf(",\n");
  }
  printf("]\n\n");
}

// Backpropagates a change to the final output through all layers in order to
// figure out a gradient that minimizes the cost for a given training data.
void backprop(network* nt, training_datum* td)
{
  size_t nb_layers = nt->nb_layers;
  size_t nb_inter = nt->nb_layers - 1;

  // Compute the list of all activations for every layer, as well as the list
  // of activations through sigmoid prime (which tells us in which direction to
  // go to increase/decrease the activation).
  nt->activations_list[0] = td->input;
  for (size_t i = 1; i < nb_layers; ++i)
  {
    size_t activations_length = nt->sizes[i];
    double* prev_activations = nt->activations_list[i - 1];
    double* activations_raw = nt->activations_raw_list[i - 1];
    double* activations = nt->activations_list[i];
    double* activations_prime = nt->activations_prime_list[i - 1];

    feedforward_step(nt, prev_activations, i - 1, activations_raw);
    vector_apply(activations, sigmoid, activations_raw, activations_length);
    vector_apply(activations_prime, sigmoid_prime, activations_raw, activations_length);
  }

  double* b_delta = nt->biases_delta[nb_inter - 1];
  double* w_delta = nt->weights_delta[nb_inter - 1];
  double* activations = nt->activations_list[nb_layers - 1];
  double* prev_activations = nt->activations_list[nb_layers - 2];
  size_t size = nt->sizes[nb_layers - 1];
  size_t prev_size = nt->sizes[nb_layers - 2];

  // Cost derivative
  for (size_t i = 0; i < size; ++i)
  {
    // Cross entropy derivative
    b_delta[i] = activations[i] - td->output[i];
  }

  dot_ti(w_delta, b_delta, prev_activations, size, 1, prev_size);

  // Δ = cost_derivative(acti[-1], output)
  // ∇biases[-1] = Δ
  // ∇weights[-1] = dot(Δ, transpose(acti[-2]))
  // In our case, we dot(transpose(Δ), acti[-2]) since our matrices are
  // already transposed in memory.
  // Which is strange, since we don't need to do it in the loop below.
  // @TODO: Investigate.

  double* prev_b_delta = b_delta;
  for (size_t i = 2; i < nb_layers; ++i)
  {
    double* prev_activations = nt->activations_list[nb_layers - i - 1];
    double* activations_prime = nt->activations_prime_list[nb_inter - i];
    double* next_weights = nt->weights[nb_inter - i + 1];
    double* b_delta = nt->biases_delta[nb_inter - i];
    double* w_delta = nt->weights_delta[nb_inter - i];
    size_t prev_size = nt->sizes[nb_inter - i];
    size_t size = nt->sizes[nb_inter - i + 1];
    size_t next_size = nt->sizes[nb_inter - i + 2];

    dot_ti(b_delta, next_weights, prev_b_delta, size, next_size, 1);
    vector_multiply(b_delta, b_delta, activations_prime, size);
    dot_it(w_delta, b_delta, prev_activations, size, 1, prev_size);
    prev_b_delta = b_delta;
    // Δ = dot(transpose(weights[-i + 1]), Δ) * acti'[-i]
    // ∇biases[-i] = Δ
    // ∇weights[-i] = dot(Δ, transpose(acti[-i - 1]))
  }
}

// Train the network on the provided training data with the provided learning
// rate.
void train(network* nt, training_datum** batch, size_t batch_length,
           double learning_rate, double weight_decay, size_t td_length)
{
  size_t nb_inter = nt->nb_layers - 1;

  for (size_t i = 0; i < batch_length; ++i)
  {
    training_datum* td = batch[i];
    backprop(nt, td);
    for (size_t i = 0; i < nb_inter; ++i)
    {
      vector_add(nt->biases_grad[i], nt->biases_delta[i], nt->biases_delta[i],
                 nt->sizes[i + 1]);
      vector_add(nt->weights_grad[i], nt->weights_delta[i],
                 nt->weights_delta[i], nt->sizes[i] * nt->sizes[i + 1]);
    }
  }

  // `grad` now contains the changes we need to apply to our network's biases
  // and weights in order to minimize the cost for our training data.

  // L2 regularization
  double l2_factor = 1. - learning_rate * (weight_decay / (double)td_length);
  double batch_factor = -learning_rate / (double)batch_length;
  for (size_t i = 0; i < nb_inter; ++i)
  {
    // @TODO: Optimization: could use `vector_scalar_multiply` and `vector_add` here,
    // but it would imply two more loops.
    for (size_t j = 0; j < nt->sizes[i + 1]; ++j)
    {
      nt->biases[i][j] += batch_factor * nt->biases_grad[i][j];
    }
    for (size_t j = 0; j < nt->sizes[i] * nt->sizes[i + 1]; ++j)
    {
      nt->weights[i][j] = l2_factor * nt->weights[i][j] +
                          batch_factor * nt->weights_grad[i][j];
    }
  }
}

double cross_entropy(double* expected, double* actual, size_t length)
{
  double sum = 0.;
  for (size_t i = 0; i < length; ++i)
  {
    double y = expected[i];
    double a = actual[i];
    double res = -y * log(a) - (1 - y) * log(1 - a);
    sum += isnan(res) ? 0. : res;
  }
  return sum;
}

double total_cost(network* nt, training_datum** training_data,
                  size_t training_data_length, double weight_decay)
{
  double cost = 0.;
  size_t activations_length = nt->sizes[nt->nb_layers - 1];
  double* activations = malloc(activations_length * sizeof (double));
  for (size_t i = 0; i < training_data_length; ++i)
  {
    training_datum* td = training_data[i];
    feedforward(nt, td->input, activations);
    cost += cross_entropy(td->output, activations, activations_length);
  }
  free(activations);
  double sum = 0.;
  for (size_t i = 0; i < nt->nb_layers - 1; ++i)
  {
    size_t n = nt->sizes[i] * nt->sizes[i + 1];
    for (size_t j = 0; j < n; j++)
    {
      sum += pow(nt->weights[i][j], 2);
    }
  }
  cost += 0.5 * weight_decay * sum;
  cost /= (double)training_data_length;
  return cost;
}

// Stochastic gradient descent.
void sgd(network* nt, training_datum** training_data,
         size_t training_data_length, unsigned long long epochs,
         size_t mini_batch_size, double learning_rate, double weight_decay,
         char output_path[], char output_csv[])
{
  FILE* fp = fopen(output_csv, "w");
  fprintf(fp, "Epoch;Cost;Time\n");
  training_datum** td = malloc(training_data_length * sizeof (training_datum*));
  char nt_name[255];
  for (unsigned long long epoch = 0; epoch < epochs; epoch++)
  {
    struct timespec tstart;
    struct timespec tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    warnx("Epoch %llu/%llu", epoch, epochs);
    shuffle(td, training_data, training_data_length, sizeof (training_datum*));
    for (size_t i = 0; i < training_data_length; i += mini_batch_size)
    {
      train(nt, td + i, MIN(mini_batch_size, training_data_length - i),
            learning_rate, weight_decay, training_data_length);
    }

    clock_gettime(CLOCK_MONOTONIC, &tend);

    double cost = total_cost(nt, training_data, training_data_length,
                             weight_decay);

    fprintf(fp, "%llu;%f;%.5f\n", epoch, cost,
           ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

    warnx("Done, cost %f", cost);
    sprintf(nt_name, "%s/epoch%llu.network", output_path, epoch);
    warnx("Saving network to file: %s", nt_name);
    network_save(nt, nt_name);
    warnx("Saved");
  }
  free(td);
  fclose(fp);
}

void network_save(network* nt, char filename[])
{
  FILE* fp = fopen(filename, "w");
  fwrite(&nt->nb_layers, sizeof (size_t), 1, fp);
  fwrite(nt->sizes, nt->nb_layers * sizeof (size_t), 1, fp);
  for (size_t j = 0; j < nt->nb_layers - 1; ++j)
  {
    fwrite(nt->weights[j], nt->sizes[j] * nt->sizes[j + 1] * sizeof (double),
           1, fp);
  }
  for (size_t j = 0; j < nt->nb_layers - 1; ++j)
  {
    fwrite(nt->biases[j], nt->sizes[j + 1] * sizeof (double), 1, fp);
  }
  fclose(fp);
}

network* network_load(char filename[])
{
  FILE* fp = fopen(filename, "r");
  size_t nb_layers;
  fread(&nb_layers, sizeof (size_t), 1, fp);
  size_t* sizes = malloc(nb_layers * sizeof (size_t));
  fread(sizes, nb_layers * sizeof (size_t), 1, fp);

  network* nt = create_network(sizes, nb_layers);

  for (size_t i = 0; i < nt->nb_layers - 1; ++i)
  {
    fread(nt->weights[i], nt->sizes[i] * nt->sizes[i + 1] * sizeof (double),
          1, fp);
  }

  for (size_t i = 0; i < nt->nb_layers - 1; ++i)
  {
    fread(nt->biases[i], nt->sizes[i + 1] * sizeof (double), 1, fp);
  }

  fclose(fp);
  return nt;
}