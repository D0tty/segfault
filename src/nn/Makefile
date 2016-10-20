CC=clang

CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -Werror -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image -lm

SRC= ../util/shuffle.c ../util/gaussrand.c ../util/vector.c ../util/debug.c network.c main.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}

all: main

-include ${DEP}

main: ${OBJ}

clean cl:
	rm -f ${OBJ} ${DEP} main

# END
