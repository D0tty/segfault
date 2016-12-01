#CC=clang
CC=gcc

CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -Werror -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image -lm

SRC= ../util/vector.c ../util/gaussrand.c ../util/shuffle.c ../util/qsort.c \
		 ../nn/network.c ../util/debug.c train.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

all: train

-include ${DEP}

train: ${OBJ}

clean:
	rm -f ${BIN}

# END
