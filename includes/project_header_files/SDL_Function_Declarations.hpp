#ifndef SDL_DECLARATIONS_HPP
#define SDL_DECLARATIONS_HPP

bool init(SDL_Window*&, SDL_Renderer*&, SDL_Surface*&);
void close(SDL_Window*&, SDL_Renderer*&, SDL_Surface*&);
std::string getDeviceName();

bool handleKeyDown(SDL_Window*, SDL_KeyboardEvent*, std::string&);
std::string handleTextInput(SDL_Window*, SDL_TextInputEvent*, std::string&);

#endif