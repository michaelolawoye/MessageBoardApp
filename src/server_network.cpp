#include "../includes/Project_Includes.hpp"

int createSocket(int);

int main(int argc, char* argv[]) {

	int listenfd = createSocket(MY_PORT);

	if (listen(listenfd, 10) == -1) {
		printf("listen failed\n");
		return 1;
	}
	printf("Socket: %d\n", listenfd);

	return 0;
}


int createSocket(int port) {

	struct addrinfo hints;
	struct addrinfo *myaddrinfo, *currAddr;
	int listenfd;
	int error_val;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	char ch_port[50];

	sprintf(ch_port, "%d", port);

	if (error_val = getaddrinfo(nullptr, ch_port, &hints, &myaddrinfo); error_val != 0) {
		printf("getaddrinfo() failed. Error value: %d\n", error_val);
		return 1;
	}

	for  (currAddr = myaddrinfo; currAddr != nullptr; currAddr = currAddr->ai_next) {

		if (listenfd = socket(currAddr->ai_family, currAddr->ai_socktype, currAddr->ai_protocol); listenfd == -1) {
			printf("socket() failed. Trying next one...\n");
			continue;
		}

		if (bind(listenfd, currAddr->ai_addr, currAddr->ai_addrlen) == -1) {
			printf("bind() failed. Trying next one...\n");
			continue;
		}

		break;
	}

	freeaddrinfo(myaddrinfo);

	return listenfd;	
}