## Simple SDL mini code

#CC=clang
CC=gcc

CPPFLAGS= `pkg-config --cflags gtk+-3.0`
CFLAGS= -Wall -std=c99 -ggdb
LDFLAGS=
LDLIBS= `pkg-config --libs gtk+-3.0`

SRC= gtk.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
BIN= ${SRC:.c=}

#all: gtkpxe gtk
all: gtk
	make pro

-include ${DEP}

gtk: ${OBJ}

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


#PXE IS SHIT
#gtkpxe: ${BIN}
#CPPFLAGS=-pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/harfbuzz -I/usr/include/libdrm -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
#LDLIBS=-lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0


# END
