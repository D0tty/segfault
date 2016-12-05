## Simple SDL mini code

#CC=clang
CC=gcc

CPPFLAGS=`pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -pedantic -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL -lSDL_image -lm

SRC= main.c sdl.c quick_sort.c list.c ../nn/network.c ../util/vector.c ../util/gaussrand.c ../util/debug.c ../util/shuffle.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

all: main
	make pro

-include ${DEP}

main: ${OBJ}

proper pro:
	rm -f  ${OBJ} ${DEP}

clean cl: proper
	rm -f *~ ${BIN}

# END
