#CC=clang
CC=gcc

CPPFLAGS=
CFLAGS= -Wall -Wextra -Werror -std=c99 -ggdb
LDFLAGS=
LDLIBS= -lm

SRC= ../util/vector.c ../util/gaussrand.c ../util/shuffle.c ../nn/network.c \
		 ../util/debug.c test_vector.c test_network.c sout.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

all: test pro run #test pro

-include ${DEP}

test: ${OBJ}

run: all
	clear;./test

proper pro:                                                                     
	  rm -f  ${OBJ} ${DEP}

clean cl: pro
	rm -f  ${BIN}

# END
