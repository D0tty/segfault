#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../util/vector.h"
#include "../util/debug.h"

int vector_equal(double* v1, double* v2, size_t length)
{
  for (size_t i = 0; i < length; ++i)
  {
    if (v1[i] != v2[i])
    {
      return 0;
    }
  }
  return 1;
}

void test_vector_add()
{
  double vec1[] = { 1, 2, 3, 4 };
  double vec2[] = { 3, 2, -1, 7 };
  double *result = malloc(4 * sizeof (double));
  vector_add(result, vec1, vec2, 4);

  assert(vector_equal(vec1, (double[]){ 1, 2, 3, 4 }, 4));
  assert(vector_equal(vec2, (double[]){ 3, 2, -1, 7 }, 4));
  assert(vector_equal(result, (double[]){ 4, 4, 2, 11 }, 4));

  free(result);
  printf("vector_add: ok\n");
}

void test_vector_substract()
{
  double vec1[] = { 1, 2, 3, 4 };
  double vec2[] = { 3, 2, -1, 7 };
  double *result = malloc(4 * sizeof (double));
  vector_substract(result, vec1, vec2, 4);

  assert(vector_equal(vec1, (double[]){ 1, 2, 3, 4 }, 4));
  assert(vector_equal(vec2, (double[]){ 3, 2, -1, 7 }, 4));
  assert(vector_equal(result, (double[]){ -2, 0, 4, -3 }, 4));

  free(result);
  printf("vector_substract: ok\n");
}

void test_vector_multiply()
{
  double vec1[] = { 1, 2, 3, 4 };
  double vec2[] = { 3, 2, -1, 7 };
  double *result = malloc(4 * sizeof (double));
  vector_multiply(result, vec1, vec2, 4);

  assert(vector_equal(vec1, (double[]){ 1, 2, 3, 4 }, 4));
  assert(vector_equal(vec2, (double[]){ 3, 2, -1, 7 }, 4));
  assert(vector_equal(result, (double[]){ 3, 4, -3, 28 }, 4));

  free(result);
  printf("vector_multiply: ok\n");
}

double negate(double n)
{
  return -n;
}

void test_vector_apply()
{
  double vec1[] = { 1, 2, 3, 4 };
  double *result = malloc(4 * sizeof (double));
  vector_apply(result, &negate, vec1, 4);

  assert(vector_equal(vec1, (double[]){ 1, 2, 3, 4 }, 4));
  assert(vector_equal(result, (double[]){ -1, -2, -3, -4 }, 4));

  free(result);
  printf("vector_apply: ok\n");
}

void test_dot()
{
  double vec1[] = {
    1, 2, 3,
    4, 5, 6,
  };
  double vec2[] = {
    -1, 6, -4,
    3, 3, 1,
    5, -2, -3,
  };
  double *result = malloc(9 * sizeof (double));
  dot(result, vec1, vec2, 2, 3, 3);

  assert(vector_equal(vec1, (double[]){
    1, 2, 3,
    4, 5, 6,
  }, 6));
  assert(vector_equal(vec2, (double[]){
    -1, 6, -4,
    3, 3, 1,
    5, -2, -3,
  }, 9));
  assert(vector_equal(result, (double[]){
    20, 6, -11,
    41, 27, -29
  }, 6));

  free(result);
  printf("dot: ok\n");
}

void test_dot_it()
{
  double vec1[] = {
    1, 2, 3,
    4, 5, 6,
  };
  double vec2[] = {
    -1, 6, -4,
    3, 3, 1,
    5, -2, -3,
  };
  double vec2_t[] = {
    -1, 3, 5,
    6, 3, -2,
    -4, 1, -3,
  };
  double *result = malloc(9 * sizeof (double));
  double *result2 = malloc(9 * sizeof (double));
  dot_it(result, vec1, vec2, 2, 3, 3);
  dot(result2, vec1, vec2_t, 2, 3, 3);

  assert(vector_equal(vec1, (double[]){
    1, 2, 3,
    4, 5, 6,
  }, 6));
  assert(vector_equal(vec2, (double[]){
    -1, 6, -4,
    3, 3, 1,
    5, -2, -3,
  }, 9));
  assert(vector_equal(result, result2, 6));

  free(result);
  free(result2);
  printf("dot_it: ok\n");
}

void test_dot_ti()
{
  double vec1[] = {
    1, 2, 3,
    4, 5, 6,
  };
  double vec1_t[] = {
    1, 3,
    2, 5,
    3, 6,
  };
  double vec2[] = {
    -1, 6, -4,
    3, 3, 1,
  };
  double *result = malloc(9 * sizeof (double));
  double *result2 = malloc(9 * sizeof (double));
  dot_ti(result, vec1, vec2, 3, 2, 3);
  dot(result2, vec1_t, vec2, 3, 2, 3);

  assert(vector_equal(vec1, (double[]){
    1, 2, 3,
    4, 5, 6,
  }, 6));
  assert(vector_equal(vec2, (double[]){
    -1, 6, -4,
    3, 3, 1,
  }, 6));
  assert(vector_equal(result, result2, 4));

  free(result);
  free(result2);
  printf("dot_ti: ok\n");
}

void test_vector()
{
  test_vector_add();
  test_vector_substract();
  test_vector_multiply();
  test_vector_apply();
  test_dot();
  test_dot_it();
}
