
sdl-triangles: sdl-triangles.c
	$(CC) -o sdl-triangles sdl-triangles.c

clean:
	rm -f *.o sdl-triangles

