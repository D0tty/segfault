# include <stdlib.h>
# include <stdio.h>

# include "../img/img.h"
# include "../img/decoupage.h"
# include "../nn/network.h"


/*
 * if len == 0 return a pointer to the buffer, NULL if buffer not init
 * if len != 0 realloc the buffer (no dada ereased and can increase len
 */
wchar_t* get_buffer(size_t len);

/*
 * put the char c in the buffer
 * if the buffer is full after adding c; return 1 else 0
 */
int put_char(char c);

/*
 * called with the correct page structure, it fills the txt buffer
 * to we can use this buffer to print / show the converted text
 */
void img_to_buff(network *nt, struct page *pg);
