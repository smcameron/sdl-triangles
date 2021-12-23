
PKG_CONFIG?=pkg-config

SDLLIBS:=$(shell $(PKG_CONFIG) sdl2 --libs) -lSDL2_image
SDLCFLAGS:=$(shell $(PKG_CONFIG) sdl2 --cflags)

MYCFLAGS=-Wall -Wextra --pedantic ${SDLCFLAGS}

sdl-triangles: sdl-triangles.c
	$(CC) ${MYCFLAGS} -o sdl-triangles sdl-triangles.c ${SDLLIBS}

clean:
	rm -f *.o sdl-triangles

