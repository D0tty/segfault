## Simple SDL mini code

#CC=clang
CC=gcc

CPPFLAGS= `pkg-config --cflags gtk+-3.0 gmodule-2.0 --cflags` -lSDL -lSDL_image
CFLAGS= -Wall -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs gtk+-3.0 gmodule-2.0` -lSDL_image -lSDL -lm -lrt

SRC= ../v2/world.c ../v2/quick_sort.c ../v2/list.c ../v2/sdl.c ../img/img.c \
		 ../img/decoupage.c main.c\
		 ../nn/network.c ../util/vector.c ../util/gaussrand.c ../util/debug.c \
		 ../util/shuffle.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

#all: gtkpxe gtk

all: main
	make pro

-include ${DEP}

train:
	make ../train/train

main: ${OBJ}

gtk: ${OBJ}

test_callback:
	$(CC) $(CFLAGS) `pkg-config --cflags gtk+-3.0 gmodule-2.0` -o test_callback test_callback.c $(LDLIBS)

test:
	./gtk

degub gdb:
	gdb gtk -q

valgrind valg:
	valgrind ./gtk

proper pro:
	rm -f  ${OBJ} ${DEP}

clean cl: proper
	rm -f *~ ${BIN}

# END
