#ifndef NETWORK_H
#define NETWORK_H

typedef struct network {
	size_t sizes_length;
	size_t* sizes;
  double** biases;
  double** weights;
} network;

typedef struct training_datum {
  double* input;
  double* output;
} training_datum;

network* create_network(size_t* sizes, size_t sizes_length);

void free_network(network* nt);

void print_network(network* nt, int with_edges);

double* feedforward(network* nt, double* input);
//
// void train(network* nt, training_datum** training_data,
//            size_t training_data_length, double eta);
//
// // Stochastic gradient descent
// void sgd(network* nt, training_datum** training_data,
//          size_t training_data_length, unsigned epochs, size_t mini_batch_size,
//          double eta);

#endif
