## Simple SDL mini code

#CC=clang
CC=gcc

CPPFLAGS=
CFLAGS= -Wall -Wextra -Werror -std=c99 -ggdb
LDFLAGS=
LDLIBS= -lSDL -lSDL_image -lm -lrt

SRC= ../util/vector.c ../util/gaussrand.c ../util/shuffle.c ../util/qsort.c \
     ../nn/network.c ../util/debug.c ../img/img.c ../img/decoupage.c test.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

all: test
	make pro

-include ${DEP}

test: ${OBJ}

proper pro:
	rm -f  ${OBJ} ${DEP}

clean cl: proper
	rm -f *~ ${BIN}

# END
