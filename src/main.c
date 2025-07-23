#include "../includes/SDL_Includes.h"
#include "../includes/SDL_Constants.h"
#include "../includes/SDL_Declarations.h"


int main(int argc, char* argv[]) {

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	SDL_Surface* img = NULL;
	SDL_Surface* opt_img = NULL;
	SDL_Rect screenRect;

	screenRect.x = 0;
	screenRect.y = 0;
	screenRect.w = SCREEN_WIDTH;
	screenRect.h = SCREEN_HEIGHT;

	if (!init(&window, &screenSurface)) {
		return 1;
	}

	screenSurface = SDL_GetWindowSurface(window);

	printf("exec path: %s\n", SDL_GetBasePath());
	
	char path[] = "./assets/simplebmp.bmp";
	img = SDL_LoadBMP(path);
	if (img == NULL) {
		printf("Couldn't load this image: %s . Error: %s.\n", path, SDL_GetError());
		return 1;
	}

	opt_img = SDL_ConvertSurface(img, screenSurface->format, 0);
	SDL_FreeSurface(img);

	if (SDL_BlitScaled(opt_img, NULL, screenSurface, &screenRect) == -1) {
		printf("Failed to blit image. Error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_UpdateWindowSurface(window);

	SDL_Event e;
	bool quit = false;
	int colour = 0xFF;

	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) 
				quit = true;

			if (e.type == SDL_KEYDOWN) {

				switch(e.key.keysym.sym) {

					case SDLK_UP:
						printf("Pressed up\n");
						break;
					case SDLK_DOWN:
						printf("Pressed down\n");
						break;
					case SDLK_LEFT:
						printf("Pressed left\n");
						break;
					case SDLK_RIGHT:
						printf("Pressed right\n");
						break;
					default:
						printf("idk what you pressed\n");
						break;
				}
			}
		}
	}

	SDL_FreeSurface(opt_img);
	img = NULL;
	close(window);

	return 0;
}


bool init(SDL_Window** window, SDL_Surface** surface) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initalize SDL. Error: %s\n", SDL_GetError());
		return false;
	}

	*window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (*window == NULL) {
		printf("Failed to create window. Error: %s\n", SDL_GetError());
		return false;
	}

	if ((*surface = SDL_GetWindowSurface(*window)) == NULL) {
		printf("Couldn't get Window surface. Error: %s", SDL_GetError());
		return false;
	}



	SDL_FillRect(*surface, NULL, SDL_MapRGB((*surface)->format, 0xFF, 0xFF, 0xFF));

	SDL_UpdateWindowSurface(*window);


	return true;
}


void close(SDL_Window* window) {

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();

}

