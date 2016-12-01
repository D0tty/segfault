# include <stdlib.h>
# include <stdio.h>

# include "../img/img.h"
# include "../img/decoupage.h"
# include "../nn/network.h"

/*
 * if len == 0 return a pointer to the buffer, NULL if buffer not init
 * if len != 0 realloc the buffer (no dada ereased and can increase len
 */
char* get_buffer(size_t len)
{
  static char *buf = NULL;
  
  if ( len )
  {
    buf = realloc( buf, len * sizeof(char) );
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
}

void img_to_buff(/*pointeur sur network, */ struct page *pg)
{
  size_t len = paragraph_compt(pg, 1);
  char *buffer = get_buffer(len);
  char car = NULL;

  while(pg != NULL)
  {
    struct paragraph prg = pg->current_paragraph;
    while(prg != NULL)
    {
      struct line *ln = prg->current_line;
      while(ln != NULL)
      {
        /*car = appel du NN sur ln->current_char;*/
        put_char(car);
        ln = ln->next_char;
      }
      put_char('\n');
      prg = prg->next_line;
    }
    put_char('\t');
    pg = pg->next_paragraph;
  }
}
