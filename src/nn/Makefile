CC=clang

CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -Werror -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image

SRC= ../util/shuffle.c ../util/gaussrand.c network.c main.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}

all:

-include ${DEP}

main: ${OBJ}

clean cl:
	rm -f ${OBJ} ${DEP} main

# END
