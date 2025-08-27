#include "../includes/Project_Includes.hpp"

int createListenSocket(int);
int pollConnections();
int createCommunicationSocket(int);

#define debugprintf printf

constexpr int SOCKET_COUNT{2};
constexpr int MAXMSGSIZE{256};


class Server{

	private:
		int clsListenfd;
		int clsCommfd;
		int clsPort;
		fd_set* clsMasterClientfds; // master fd_set to keep track of all fds

		// fd_sets for select() arguments
		fd_set* clsClientRfds;
		fd_set* clsClientWfds;
		fd_set* clsClientEfds;

		int clsMaxfd; // value for select()'s first argument

		void* get_inaddr(struct sockaddr*);
		int createListenSocket();
		int handleNewConnection();
		int deleteConnection(int clientfd);
		int handleClientData(int clientfd);
		int sendClientMessage(int clientfd);
		int handleClientError(int clientfd);

	public:
		Server(int port);
		~Server();

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

	createListenSocket();
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

	if (listen(listenfd, 10) == -1) {
		printf("Server::createListenSocket() listen failed. Errno: %d\n", errno);
		exit(1);
	}

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
	printf("Accepted connection from %s\n", ipstr);

	FD_SET(newfd, clsMasterClientfds);

	return newfd;
}

int Server::deleteConnection(int clientfd) {

	return 0;
}

int Server::handleClientData(int clientfd) {

	char recv_buffer[MAXMSGSIZE];
	std::string fullmessage;

	int bytes_recv = 0;
	while (bytes_recv >= MAXMSGSIZE) {
		if (bytes_recv = recv(clientfd, recv_buffer, MAXMSGSIZE, 0); bytes_recv == -1) {
			printf("Server::handleClientData() recv() from client socket: %d failed. Errno: %d\n", clientfd, errno);
			return -1;
		}

		if (bytes_recv == 0) { // client ended communication
			deleteConnection(clientfd);
			return 0;
		}

		fullmessage.append(recv_buffer);
	}

	printf("Message recieved from client socket %d: %s\n", clientfd, fullmessage.c_str());
	return 1;
}

int Server::sendClientMessage(int clientfd) {

	return 0;
}

int Server::handleClientError(int clientfd) {

	return 0;
}

int Server::pollConnections() {
	printf("polling...\n");
	struct timeval tv;
	tv.tv_sec = 7;
	tv.tv_usec = 0;
	clsClientRfds = clsMasterClientfds;
	clsClientWfds = clsMasterClientfds;
	clsClientEfds = clsMasterClientfds;
	select(clsMaxfd, clsClientRfds, clsClientWfds, clsClientEfds, &tv);
	printf("polled\n");
	fd_set* test = new fd_set;
	FD_SET(0, test);
	for (int i = 0; i < clsMaxfd; i++) {

		if (FD_ISSET(i, clsClientWfds)) {
			printf("x\n");
			if (i == clsListenfd) { // new client trying to connect
				handleNewConnection();
			}
			else {
				handleClientData(i);
			}
		}

		if (FD_ISSET(i, clsClientRfds)) {
			sendClientMessage(i);
		}

		if (FD_ISSET(i, clsClientEfds)) {
			handleClientError(i);
		}

	}

	return 0;
}

int main(int argc, char* argv[]) {
	printf("start of server\n");

	Server server1 = Server(MY_PORT);

	while (1) {
		server1.pollConnections();
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
