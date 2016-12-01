# include <stdlib.h>

char* alphatab()
{
  static int* tab = NULL;
  if ( ! tab )
  {
    // 20 -> 95
    // 97 -> 126
    tab = malloc ( 107 * sizeof(char) );
  }
  return tab;
}
