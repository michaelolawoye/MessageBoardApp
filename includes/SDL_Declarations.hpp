#ifndef SDL_DECLARATIONS_HPP
#define SDL_DECLARATIONS_HPP

bool init(SDL_Window*&, SDL_Renderer*&, SDL_Surface*&);
void close(SDL_Window*&, SDL_Renderer*&, SDL_Surface*&);
std::string getDeviceName();

BoardMessage* newMessage();

#endif