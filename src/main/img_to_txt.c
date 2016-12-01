# include <stdlib.h>
# include <stdio.h>

# include "../img/img.h"
# include "../img/decoupage.h"
# include "../nn/network.h"

/*
 * if size == 0 return a pointer to the buffer, NULL if buffer not init
 * if size != 0 realloc the buffer (no dada ereased and can increase size
 */
char* get_buffer(size_t size)
{
  static char *buf = NULL;
  
  if ( size )
  {
    buf = realloc( buf, size);
  }

  return buf;
}


/*
 * put the char c in the buffer
 * if the buffer is full after adding c; return 1 else 0
 */
int put_char(char c)
{
  static size_t index = 0;
  static size_t len = 0;
  len = len ? paragraph_compt(NULL, 0) : len;
  char *buf = get_buffer(0); //need to init it before put any char, obvious no ?

  buf[index++] = c; //put the char increase index

  return index >= len ? 1 : 0; //ternary to ensure 0 or 1 values 

