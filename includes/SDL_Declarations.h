#ifndef SDL_DECLARATIONS_H
#define SDL_DECLARATIONS_H

bool init(SDL_Window**, SDL_Surface**);
void close(SDL_Window*);
SDL_Renderer* createRenderer(SDL_Window*, SDL_Color);

#endif