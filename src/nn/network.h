#ifndef NETWORK_H
#define NETWORK_H

typedef struct network {
	size_t nb_layers;
	size_t* sizes;
  double** biases;
  double** weights;
} network;

typedef struct training_datum {
  double* input;
  double* output;
} training_datum;

typedef struct gradients {
	double** biases;
	double** weights;
} gradients;

network* create_network(size_t* sizes, size_t nb_layers);

void free_network(network* nt);

void print_network(network* nt, int with_edges);

void feedforward(network* nt, double* input, double* activations);

double sigmoid(double z);

void train(network* nt, training_datum** training_data,
           size_t training_data_length, double eta);

void sgd(network* nt, training_datum** training_data,
         size_t training_data_length, unsigned epochs, size_t mini_batch_size,
         double eta);

void backprop(network* nt, training_datum* td, gradients* grad);

gradients* create_gradients(network* nt);

void print_grad_net(network* nt);

void free_gradients(network* nt, gradients* grad);

#endif
