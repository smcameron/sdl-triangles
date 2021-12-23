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

#include <SDL.h>

#define SCREEN_WIDTH (16 * 64)
#define SCREEN_HEIGHT (9 * 64)

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
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
	SDL_ShowWindow(window);
	return 0;
}

