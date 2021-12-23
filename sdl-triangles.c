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

static int int_sign(int x)
{
	return (x > 0) - (x < 0);
}

static void swap_ints(int *a, int *b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

/* Scan convert a triangle with a flat top or flat bottom. It must be true that y2 == y3 */
static void scan_convert_flat_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	int tx1 = x1;
	int ty1 = y1;
	int tx2 = x1;
	int ty2 = y1;

	int ch1 = 0;
	int ch2 = 0;

	int dx1 = x2 - x1;
	int dy1 = y2 - y1;
	int dx2 = x3 - x1;
	int dy2 = y3 - y1;

	int sx1 = int_sign(dx1);
	int sx2 = int_sign(dx2);
	int sy1 = int_sign(dy1);
	int sy2 = int_sign(dy2);

	dx1 = abs(dx1);
	dy1 = abs(dy1);
	dx2 = abs(dx2);
	dy2 = abs(dy2);

	if (dy1 > dx1) {
		swap_ints(&dx1, &dy1);
		ch1 = 1;
	}
	if (dy2 > dx2) {
		swap_ints(&dx2, &dy2);
		ch2 = 1;
	}

	int e1 = 2 * dy1 - dx1;
	int e2 = 2 * dy2 - dx2;

	for (int i = 0; i <= dx1; i++) {
		SDL_RenderDrawLine(sdl_renderer, tx1, ty1, tx2, ty2);

		/* Advance tx1, ty1 via bresenham's */
		while (e1 >= 0) {
			if (ch1)
				tx1 += sx1;
			else
				ty1 += sy1;
			e1 = e1 - 2 * dx1;
		}
		if (ch1)
			ty1 += sy1;
		else
			tx1 += sx1;
		e1 = e1 + 2 * dy1;

		/* Advance tx2, ty2 via bresenham's */
		while (ty2 != ty1) {
			while (e2 >= 0) {
				if (ch2)
					tx2 += sx2;
				else
					ty2 += sy2;
				e2 = e2 - 2 * dx2;
			}
			if (ch2)
				ty2 += sy2;
			else
				tx2 += sx2;
			e2 = e2 + 2 * dy2;
		}
	}
}

/* Vertices must be ordered such that y1 <= y2 <= y3 */
static void scan_convert_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	int x4, y4;

	if (y1 == y2) { /* it's a flat-top triangle? */
		scan_convert_flat_triangle(x3, y3, x1, y1, x2, y2);
		return;
	}
	if (y2 == y3) { /* it's a flat-bottom triangle? */
		scan_convert_flat_triangle(x1, y1, x2, y2, x3, y3);
		return;
	}
	/* Cut the triangle into two with a horizontal line through (x2, y2)
	 * to make a flat-topped and flat-bottomed pair of triangles
	 */
	x4 = (int) (x1 + ((float) (y2 - y1) / (float) (y3 - y1)) * (x3 - x1));
	y4 = y2;
	scan_convert_flat_triangle(x1, y1, x2, y2, x4, y4);
	scan_convert_flat_triangle(x3, y3, x2, y2, x4, y4);
}

static void draw_filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	/* Scan convert triangle but first, sort vertices by y coord, top to bottom */
	if (y1 <= y2)
		if (y1 <= y3)
			if (y2 <= y3)
				scan_convert_triangle(x1, y1, x2, y2, x3, y3);
			else
				scan_convert_triangle(x1, y1, x3, y3, x2, y2);
		else
			scan_convert_triangle(x3, y3, x1, y1, x2, y2);
	else
		if (y1 >= y3)
			if (y2 >= y3)
				scan_convert_triangle(x3, y3, x2, y2, x1, y1);
			else
				scan_convert_triangle(x2, y2, x3, y3, x1, y1);
		else
			scan_convert_triangle(x2, y2, x1, y1, x3, y3);
}

static void draw_unfilled_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
	SDL_RenderDrawLine(sdl_renderer, x2, y2, x3, y3);
	SDL_RenderDrawLine(sdl_renderer, x3, y3, x1, y1);
}

static void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int filled)
{
	if (filled)
		draw_filled_triangle(x1, y1, x2, y2, x3, y3);
	else
		draw_unfilled_triangle(x1, y1, x2, y2, x3, y3);
}

static void clear_window(void)
{
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdl_renderer);
}

static void draw_triangles(void)
{
	int i, x1, y1, x2, y2, x3, y3, r, g, b, f;

	for (i = 0; i < NTRIANGLES; i++) {
		r = rand() % 127 + 128;
		g = rand() % 127 + 128;
		b = rand() % 127 + 128;
		f = rand() % 2;
		SDL_SetRenderDrawColor(sdl_renderer, r, g, b, SDL_ALPHA_OPAQUE);
		random_point(&x1, &y1);
		random_point(&x2, &y2);
		random_point(&x3, &y3);
		draw_triangle(x1, y1, x2, y2, x3, y3, f);
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

