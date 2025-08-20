#ifndef NETWORKING_ALL_INCLUDES_HPP
#define NETWORKING_ALL_INCLUDES_HPP

#ifdef _WIN32 
	#include "Networking_Windows_Includes.hpp"
#else
	#include "Networking_Linux_Includes.hpp"
#endif

#define MY_PORT 5050
#include  <stdlib.h>
#include <signal.h>



void* get_inaddr(struct sockaddr* sa) {

	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


#endif