#ifndef VECTOR_H
#define VECTOR_H

void vector_add(double* dst, double*a, double* b, size_t length);

void vector_substract(double* dst, double* a, double* b, size_t length);

void vector_multiply(double* dst, double* a, double* b, size_t length);

void vector_apply(double* dst, double (*f)(double), double* src, size_t length);

void dot(double* dst, double* a, double* b, size_t w_a, size_t h_a_w_b, size_t h_b);

void dot_it(double* dst, double* a, double* b, size_t w_a, size_t h_a_w_b, size_t h_b);

void dot_ti(double* dst, double* a, double* b, size_t w_a, size_t h_a_w_b, size_t h_b);

#endif
