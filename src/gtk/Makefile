## Simple SDL mini code

#CC=clang
CC=gcc

CPPFLAGS= `pkg-config --cflags gtk+-3.0 gmodule-2.0`
CFLAGS= -Wall -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs gtk+-3.0 gmodule-2.0`

SRC= mikatest.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

#all: gtkpxe gtk
all: mikatest
	make pro

-include ${DEP}

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
