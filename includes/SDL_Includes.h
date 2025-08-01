#ifndef SDL_INCLUDES_H
#define SDL_INCLUDES_H

#include <stdio.h>
#include <iostream>
#include <string>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#endif