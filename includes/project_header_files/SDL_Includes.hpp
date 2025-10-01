#ifndef SDL_INCLUDES_HPP
#define SDL_INCLUDES_HPP

// C/C++ standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <csignal>
#include <unordered_map>
#include <queue>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif


// SDL3 libraries
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#endif