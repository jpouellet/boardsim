#include <err.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Window *window;
SDL_Surface *screen;
SDL_Surface *board;
SDL_Surface *led;

void gui_init(void) {
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		err(1, "SDL_Init: %s", SDL_GetError());

	int flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags)
		err(1, "IMG_Init: %s", IMG_GetError());

	board = IMG_Load("DE2.png");
	if (board == NULL)
		err(1, "IMG_Load: %s", IMG_GetError());

	led = IMG_Load("led.png");
	if (led == NULL)
		err(1, "IMG_Load: %s", IMG_GetError());

	window = SDL_CreateWindow("board", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, board->w, board->h, SDL_WINDOW_SHOWN);
	if (window == NULL)
		err(1, "SDL_CreateWindow: %s", SDL_GetError());

	screen = SDL_GetWindowSurface(window);
	if (screen == NULL)
		err(1, "SDL_GetWindowSurface: %s", SDL_GetError());
}

void gui_render(void) {
	if (SDL_BlitSurface(board, NULL, screen, NULL) < 0)
		err(1, "SDL_BlitSurface (board): %s", SDL_GetError());

	SDL_Rect dr;
	dr.x = board->w / 2;
	dr.y = board->h / 2;
	dr.w = led->w;
	dr.w = led->h;

	if (SDL_BlitSurface(led, NULL, screen, &dr) < 0)
		err(1, "SDL_BlitSurface (led): %s", SDL_GetError());

	if (SDL_UpdateWindowSurface(window) < 0)
		err(1, "SDL_UpdateWindowSurface: %s", SDL_GetError());
}

int
main(int argc, char *argv[])
{
	gui_init();
	gui_render();
	SDL_Delay(2000);
}
