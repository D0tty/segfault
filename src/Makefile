## Simple SDL mini code
 
CC=clang
 
CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -Werror -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image
 
SRC= img.c
OBJ= ${SRC:.c=.o}
PGR= ${SRC:.c=}

all: img
 
main: ${OBJ}
 
clean cl:
	rm -f *~ *.o
	rm -f ${PGR}
 
# END
