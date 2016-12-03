# include "img_to_txt.h"
# include "../nn/network.h"


int maint(int argc, char *argv[])
{
  
  //check on args


  network* nt = network_load(network_path);

  free_network(nt);
}
