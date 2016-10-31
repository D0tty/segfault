#CC=clang
CC=gcc

CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -Werror -std=c99 -O2
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL -lSDL_image -lm

SRC= ../util/shuffle.c ../util/gaussrand.c ../util/vector.c ../util/debug.c \
		 ../nn/network.c test_network.c sout.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}

all: sout

-include ${DEP}

sout: ${OBJ}

clean cl:
	rm -f ${OBJ} ${DEP} sout

# END
