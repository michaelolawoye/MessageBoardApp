#ifndef NETWORKING_ALL_INCLUDES_HPP
#define NETWORKING_ALL_INCLUDES_HPP

#ifdef _WIN32 
	#include "Networking_Windows_Includes.hpp"
#else
	#include "Networking_Linux_Includes.hpp"
#endif

#define MY_PORT 5050
#include  <stdlib.h>

#endif