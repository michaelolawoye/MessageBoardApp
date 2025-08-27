#include "../includes/Project_Includes.hpp"

int createListenSocket(int);
int pollConnections();
int createCommunicationSocket(int);


constexpr int SOCKET_COUNT{2};


class Server{

	private:
		int clsListenfd;
		int clsCommfd;
		int clsPort;
		fd_set* clsMasterClientfds;
		fd_set* clsClientRfds;
		fd_set* clsClientWfds;
		fd_set* clsClientEfds;

		int clsMaxfd; // value for select()'s first argument

	public:
		Server(int port);
		~Server();

		void* get_inaddr(struct sockaddr*);
		int createListenSocket();
		int handleNewConnection();
		int handleClientData(int clientfd);
		int pollConnections();
};

void* Server::get_inaddr(struct sockaddr* sa) {

	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Server::Server(int port): clsPort{port} {

	clsMasterClientfds = new fd_set;
	clsClientRfds = new fd_set;
	clsClientWfds = new fd_set;
	clsClientEfds = new fd_set;
	FD_ZERO(clsMasterClientfds);
	FD_ZERO(clsClientRfds);
	FD_ZERO(clsClientWfds);
	FD_ZERO(clsClientEfds);
}

Server::~Server() {

	FD_ZERO(clsMasterClientfds);
	FD_ZERO(clsClientRfds);
	FD_ZERO(clsClientWfds);
	FD_ZERO(clsClientEfds);
	delete clsMasterClientfds;
	delete clsClientRfds;
	delete clsClientWfds;
	delete clsClientEfds;
}

int Server::createListenSocket() {

	struct addrinfo hints, *myaddrinfo, *curraddr;

	int listenfd; // fd to be returned
	int error_val, sockoptnum;

	char char_port[10];

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	sprintf(char_port, "%d", clsPort);

	if (error_val = getaddrinfo(nullptr, char_port, &hints, &myaddrinfo); error_val != 0) {
		printf("Server::createListenSocket() getaddrinfo() failed. Error value: %d\n", error_val);
		return -1;
	}

	for (curraddr = myaddrinfo; curraddr != nullptr; curraddr = curraddr->ai_next) {

		if (listenfd = socket(curraddr->ai_family, curraddr->ai_socktype, curraddr->ai_protocol); listenfd < 0) {
			printf("Server::createListenSocket() socket() failed, trying next one... Errno: %d\n", errno);
			continue;
		}

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sockoptnum, sizeof(int)) == -1) {
			printf("Server::createListenSocket() setsockopt() failed. Exiting program. Errno: %d\n", errno);
			exit(1);
		}

		if (bind(listenfd, curraddr->ai_addr, curraddr->ai_addrlen) == -1) {
			printf("Server::createListenSocket() bind() failed, trying next socket... Errno: %d\n", errno);
			continue;
		}
	}

	if (listenfd == -1) {
		printf("Server::createListenSocket() Couldn't create listen socket\n");
		return -1;
	}

	freeaddrinfo(myaddrinfo);

	printf("Server::createListenSocket() successfully created listen socket\n");

	FD_SET(listenfd, clsMasterClientfds);

	clsMaxfd = listenfd;
	clsListenfd = listenfd;

	return listenfd;
}

int Server::handleNewConnection() {

	int newfd;
	struct sockaddr_storage their_addr;
	socklen_t their_size = sizeof(struct sockaddr_storage);

	if (newfd = accept(clsListenfd, (struct sockaddr*)(&their_addr), &their_size); newfd == -1) {
		printf("Server::handleNewConnection() accept failed. Errno: %d\n", errno);
		return -1;
	}

	char ipstr[INET6_ADDRSTRLEN];

	inet_ntop(their_addr.ss_family, get_inaddr((struct sockaddr*)&their_addr), ipstr, INET6_ADDRSTRLEN);
	printf("Accepted connction from %s\n", ipstr);

	FD_SET(newfd, clsMasterClientfds);

	return newfd;
}

int Server::handleClientData(int clientfd) {

}

int Server::pollConnections() {

	clsClientRfds = clsMasterClientfds;
	clsClientWfds = clsMasterClientfds;
	clsClientEfds = clsMasterClientfds;
	select(clsMaxfd, clsClientRfds, clsClientWfds, clsClientEfds, NULL);

	

	return 0;
}

int main(int argc, char* argv[]) {
	printf("start of server\n");

	struct pollfd client_sockets[SOCKET_COUNT];
	int serverfd = -3; // unused constant
	int bytes_sent, bytes_recv;
	char recv_message[] = "nothing";
	int listenfd;
	int child_pid = -100;
	fd_set fds;

	if (listenfd = createListenSocket(MY_PORT); listenfd == -1) {
		return -1;
	}

	if (listen(listenfd, 10) == -1) {
		printf("listen() failed. Errno: %d\n", errno);
		return 1;
	}
	
	if (serverfd = createCommunicationSocket(listenfd); serverfd == -1) {
		printf("Server failed to create communication socket\n");
		return -1;
	}
	printf("Created communication socket with client\n");

	client_sockets[0].fd = serverfd;
	client_sockets[0].events = POLLIN;
	client_sockets[0].revents = 0;
	int abc = 1;
	int socks_polled = 0;
	while (1) {
		if (socks_polled = poll(client_sockets, SOCKET_COUNT, 2500); socks_polled != 0) {

			for (int i = 0; i < SOCKET_COUNT; i++) {

				if (client_sockets[i].revents & POLLIN) {
					if (bytes_recv = recv(client_sockets[i].fd, recv_message, 256, 0); bytes_recv == -1) {
						printf("Couldn't recv message. Errno: %d\n", errno);
					}
					if (abc) {
						recv_message[bytes_recv] = '\0';
						printf("Received message: %s\n", recv_message);
						abc = 0;
					}
					client_sockets[i].revents = 0;
				}
			}
		}
	}


	return 0;
}


int createCommunicationSocket(int listenfd) {

	int commfd;
	struct sockaddr_storage their_addr;
	socklen_t their_size = sizeof(struct sockaddr_storage);

	printf("Attempting to accept connection\n");	
	if (commfd = accept(listenfd, (struct sockaddr*)(&their_addr), &their_size); commfd == -1) {
		printf("accept() failed. Errno: %d\n", errno);
		return -1;
	}

	char ipstr[INET6_ADDRSTRLEN];
	inet_ntop(their_addr.ss_family, get_inaddr((struct sockaddr*)&their_addr), ipstr, INET6_ADDRSTRLEN);
	printf("Accepted connection from: %s\n", ipstr);
	return commfd;

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
