#ifndef NETWORKING_ALL_INCLUDES_HPP
#define NETWORKING_ALL_INCLUDES_HPP

#ifdef _WIN32 
	#include "Networking_Windows_Includes.hpp"
#else
	#include "Networking_Linux_Includes.hpp"
	#include "Networking_Linux_Class_Declarations.hpp"
#endif

#define MY_PORT 5050
#define CLIENT_TIMEOUT 1000
#include  <stdlib.h>
#include <signal.h>

enum {
	NO_CHANGE,
	MESSAGE_RECVD,
	MESSAGE_SEND,
	ERROR
};



void* get_inaddr(struct sockaddr* sa) {

	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::string getDeviceName() {

	char name[256];

	#ifdef _WIN32
		DWORD size = 256
		GetComputerNameA(name, &size);

	#else
		gethostname(name, sizeof(name));
	#endif

	return static_cast<std::string>(name);
}

#endif
