#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static struct timespec tstart;

void timer_start()
{
  tstart.tv_sec = 0;
  tstart.tv_nsec = 0;
  clock_gettime(CLOCK_MONOTONIC, &tstart);
}

void timer_print()
{
  struct timespec tend = { 0, 0 };
  clock_gettime(CLOCK_MONOTONIC, &tend);
  double time = ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
                ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec);
  printf("Timer: %.5f seconds\n", time);
}
