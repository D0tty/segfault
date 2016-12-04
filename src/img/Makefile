## Simple SDL mini code

#CC=clang
CC=gcc

CPPFLAGS=
CFLAGS= -Wall -Wextra -Werror -std=c99 -ggdb
LDFLAGS=
LDLIBS= -lSDL -lSDL_image -lm

SRC= img.c decoupage.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

all: img
	make pro

-include ${DEP}

img: ${OBJ}

proper pro:
	rm -f  ${OBJ} ${DEP}

clean cl: proper
	rm -f *~ ${BIN}

test: img
	./img ../../img/multicolore.png

# END
