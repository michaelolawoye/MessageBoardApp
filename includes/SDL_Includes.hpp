#ifndef SDL_INCLUDES_HPP
#define SDL_INCLUDES_HPP

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#endif