#include "../includes/Project_Includes.hpp"

int createListenSocket(int);
int findOtherSockets();
int createCommunicationSocket(int);

int main(int argc, char* argv[]) {
	printf("hello");
	int listenfd = createListenSocket(MY_PORT);

	if (listen(listenfd, 10) == -1) {
		printf("listen failed. Errno: %d\n", errno);
		return 1;
	}
	printf("Listen Socket: %d\n", listenfd);


	int newfd = createCommunicationSocket(listenfd);


	return 0;
}


int createCommunicationSocket(int listenfd) {

	if (listen(listenfd, 10) == -1) {
		printf("listen() failed. Errno: %d\n", errno);
		return -1;
	}

	int commfd = 0;
	struct sockaddr their_addr;
	socklen_t their_size = sizeof(struct sockaddr_storage);
	
	if (commfd = accept(listenfd, &their_addr, &their_size); commfd == -1) {
		printf("accept() failed. Errno: %d\n", errno);
		return -1;
	}

	return commfd;

}

int findOtherSockets(int commfd) {


	
	return 0;
}

int createListenSocket(int port) {

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
			printf("socket() failed. Trying next one... Errno: %d\n", errno);
			continue;
		}

		if (bind(listenfd, currAddr->ai_addr, currAddr->ai_addrlen) == -1) {
			printf("bind() failed. Trying next one... Errno: %d\n", errno);
			continue;
		}

		break;
	}

	freeaddrinfo(myaddrinfo);

	return listenfd;	
}