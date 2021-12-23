/*
	Copyright (C) 2021 Stephen M. Cameron
	Author: Stephen M. Cameron

	This file is part of sdl-triangles.

	sdl-triangles is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	sdl-triangles is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with sdl-triangles; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include <SDL.h>

#define SCREEN_WIDTH (16 * 64)
#define SCREEN_HEIGHT (9 * 64)
#define NTRIANGLES 20

static SDL_Renderer *sdl_renderer = NULL;

static void random_point(int *x, int *y)
{
	*x = rand() % SCREEN_WIDTH;
	*y = rand() % SCREEN_HEIGHT;
}

static void draw_unfilled_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
	SDL_RenderDrawLine(sdl_renderer, x2, y2, x3, y3);
	SDL_RenderDrawLine(sdl_renderer, x3, y3, x1, y1);
}

static void clear_window(void)
{
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdl_renderer);
}

static void draw_triangles(void)
{
	int i, x1, y1, x2, y2, x3, y3, r, g, b;

	for (i = 0; i < NTRIANGLES; i++) {
		r = rand() % 127 + 128;
		g = rand() % 127 + 128;
		b = rand() % 127 + 128;
		SDL_SetRenderDrawColor(sdl_renderer, r, g, b, SDL_ALPHA_OPAQUE);
		random_point(&x1, &y1);
		random_point(&x2, &y2);
		random_point(&x3, &y3);
		draw_unfilled_triangle(x1, y1, x2, y2, x3, y3);
	}
}

static int poll_sdl_events(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			return 1;
	}
	return 0;
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Unable to initialize SDL (Video):  %s\n", SDL_GetError());
		return 1;
	}
	SDL_Window *window = SDL_CreateWindow("SDL Triangles",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		0, 0, SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
	sdl_renderer = SDL_CreateRenderer(window, -1, 0);
	if (!sdl_renderer) {
		fprintf(stderr, "Could not create SDL renderer: %s\n", SDL_GetError());
		return 1;
	}
	clear_window();
	SDL_ShowWindow(window);
	draw_triangles();
	SDL_RenderPresent(sdl_renderer);
	while (poll_sdl_events() == 0);
	SDL_DestroyRenderer(sdl_renderer);

	return 0;
}

