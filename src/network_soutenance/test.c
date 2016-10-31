#include "test_vector.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../util/debug.h"
#include "../nn/network.h"
#include "test_network.h"

int main()
{
	test_vector();
	test_network();
	return 0;
}
