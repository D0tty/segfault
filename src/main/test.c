# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <err.h>
# include <time.h>
# include "../util/gaussrand.h"
# include "../util/vector.h"
# include "../util/debug.h"
# include "../util/shuffle.h"
# include "../util/misc.h"
# include "../nn/network.h"


char* usage(void)
{
  char *txt = "./test print <network_file> <image>\n";
  return txt;
}

int main(int argc, char *argv[])
{
  //check on args
  (--argv);
  if ( argc < 3 )
  {
    printf("%s" ,usage());
    return 2;
  }

  //network* nt = network_load(network_path);

  //free_network(nt);
  return 0;
}
