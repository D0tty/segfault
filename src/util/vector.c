#include <stdlib.h>
#include <stdio.h>

// Element-wise vector addition.
void vector_add(double* dst, double*a, double* b, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    dst[i] = a[i] + b[i];
  }
}

// Element-wise vector substraction.
void vector_substract(double* dst, double* a, double* b, size_t length)
{
  for (size_t i = 0; i < length; ++i)
  {
    dst[i] = a[i] - b[i];
  }
}

// Element-wise vector multiplication.
void vector_multiply(double* dst, double* a, double* b, size_t length)
{
  for (size_t i = 0; i < length; ++i)
  {
    dst[i] = a[i] * b[i];
  }
}

// Element-wise vector function application.
void vector_apply(double* dst, double (*f)(double), double* src, size_t length)
{
  for (size_t i = 0; i < length; ++i)
  {
    dst[i] = (*f)(src[i]);
  }
}

// Dot product between two vectors.
void dot(double* dst, double* a, double* b, size_t h_a, size_t w_a_h_b, size_t w_b)
{
  for (size_t i = 0; i < h_a; ++i)
  {
    for (size_t j = 0; j < w_b; ++j)
    {
      double res = 0;
      for (size_t k = 0; k < w_a_h_b; ++k)
      {
        res += a[i * w_a_h_b + k] * b[k * w_b + j];
      }
      dst[i * w_b + j] = res;
    }
  }
}

// Dot product between vectors (identity) . (transposed).
void dot_it(double* dst, double* a, double* b, size_t h_a, size_t w_a_h_b, size_t w_b)
{
  for (size_t i = 0; i < h_a; ++i)
  {
    for (size_t j = 0; j < w_b; ++j)
    {
      double res = 0;
      for (size_t k = 0; k < w_a_h_b; ++k)
      {
        res += a[i * w_a_h_b + k] * b[j * w_b + k];
      }
      dst[i * w_b + j] = res;
    }
  }
}


// Dot product between vectors (transposed) . (identity).
void dot_ti(double* dst, double* a, double* b, size_t h_a, size_t w_a_h_b, size_t w_b)
{
  for (size_t i = 0; i < h_a; ++i)
  {
    for (size_t j = 0; j < w_b; ++j)
    {
      double res = 0;
      for (size_t k = 0; k < w_a_h_b; ++k)
      {
        res += a[k * w_a_h_b + i] * b[k * w_b + j];
      }
      dst[i * w_b + j] = res;
    }
  }
}
