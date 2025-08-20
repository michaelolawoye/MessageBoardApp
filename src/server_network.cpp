#include "../includes/Project_Includes.hpp"

int createListenSocket(int);
int findOtherSockets();
int pollConnections();
int createCommunicationSocket(int);

 void sigchld_handler(int s) {
 	
 (void)s; // quiet unused variable warning

 // waitpid() might overwrite errno, so we save and restore it:
 int saved_errno = errno;

 while(waitpid(-1, NULL, WNOHANG) > 0);
 	errno = saved_errno;
 }


int main(int argc, char* argv[]) {
	printf("start of server\n");

	struct pollfd sockets[1];
	int serverfd;
	int bytes_sent, bytes_recv;
	char mess[] = "nothing";
	int listenfd;

	struct sigaction sa;


	if (listenfd = createListenSocket(MY_PORT); listenfd == -1) {
		return -1;
	}

	if (listen(listenfd, 10) == -1) {
		printf("listen() failed. Errno: %d\n", errno);
		return 1;
	}
	
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
		printf("something something sigaction\n");
		exit(1);
	}

	while (1) {

		if (serverfd = createCommunicationSocket(listenfd); serverfd == -1) {
			printf("Server failed to create communication socket\n");
			return -1;
		}
		printf("Created communication socket with client\n");

		if (!fork()) {
			close(listenfd);
			if (bytes_recv = recv(serverfd, mess, 1024, 0); bytes_recv == -1) {
				printf("Failed to recieve from server\n");
				return -1;
			}

			mess[bytes_recv] = '\0';
			printf("Message recieved: %s\n", mess);

			close(serverfd);
			exit(0);
		}
		close(serverfd);
	}


	return 0;
}


int createCommunicationSocket(int listenfd) {

	int commfd;
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
	int num;

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

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &num, sizeof(int)) == -1) {
			printf("setsockopt() failed. Errno: %d\n", errno);
			close(listenfd);
			return -1;
		}

		if (bind(listenfd, currAddr->ai_addr, currAddr->ai_addrlen) == -1) {
			printf("bind() failed. Trying next one... Errno: %d\n", errno);
			close(listenfd);
			continue;
		}

		break;
	}

	if (currAddr == nullptr) {
		printf("Failed to create connection\n");
		return -1;
	}

	freeaddrinfo(myaddrinfo);

	return listenfd;	
}