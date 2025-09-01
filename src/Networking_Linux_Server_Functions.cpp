int createListenSocket(int);
int pollConnections();
int createCommunicationSocket(int);


constexpr int SOCKET_COUNT{2};
constexpr int MAXMSGSIZE{256};

enum {
	NO_CHANGE,
	MESSAGE_RECVD,
	MESSAGE_SENT,
	ERROR
};


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

		Board& clsBoard;

		void* get_inaddr(struct sockaddr*);
		int createListenSocket();
		int handleNewConnection();
		int deleteConnection(int clientfd);
		int handleClientData(int clientfd);
		int sendClientMessage(int clientfd, std::string message);
		int handleClientError(int clientfd);



	public:
		Server(int port, Board& board);
		~Server();

		int pollConnections();

		int getListenFd();
		fd_set* getMasterFds();
};

void* Server::get_inaddr(struct sockaddr* sa) {

	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Server::Server(int port, Board& board): clsPort{port}, clsBoard{board} {

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

fd_set* Server::getMasterFds() { return clsMasterClientfds; }
int Server::getListenFd() { return clsListenfd; }

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
		SDL_Log("Server::createListenSocket() getaddrinfo() failed. Error value: %d\n", error_val);
		return -1;
	}

	for (curraddr = myaddrinfo; curraddr != nullptr; curraddr = curraddr->ai_next) {

		if (listenfd = socket(curraddr->ai_family, curraddr->ai_socktype, curraddr->ai_protocol); listenfd < 0) {
			SDL_Log("Server::createListenSocket() socket() failed, trying next one... Errno: %d\n", errno);
			continue;
		}

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sockoptnum, sizeof(int)) == -1) {
			SDL_Log("Server::createListenSocket() setsockopt() failed. Exiting program. Errno: %d\n", errno);
			exit(1);
		}

		if (bind(listenfd, curraddr->ai_addr, curraddr->ai_addrlen) == -1) {
			SDL_Log("Server::createListenSocket() bind() failed, trying next socket... Errno: %d\n", errno);
			continue;
		}
	}

	if (listenfd == -1) {
		SDL_Log("Server::createListenSocket() Couldn't create listen socket\n");
		return -1;
	}

	freeaddrinfo(myaddrinfo);

	SDL_Log("Server::createListenSocket() successfully created listen socket\n");

	FD_SET(listenfd, clsMasterClientfds);

	clsMaxfd = listenfd;
	clsListenfd = listenfd;

	if (listen(listenfd, 10) == -1) {
		SDL_Log("Server::createListenSocket() listen failed. Errno: %d\n", errno);
		exit(1);
	}

	return listenfd;
}

int Server::handleNewConnection() {

	int newfd;
	struct sockaddr_storage their_addr;
	socklen_t their_size = sizeof(struct sockaddr_storage);

	if (newfd = accept(clsListenfd, (struct sockaddr*)(&their_addr), &their_size); newfd == -1) {
		SDL_Log("Server::handleNewConnection() accept failed. Errno: %d\n", errno);
		return -1;
	}

	char ipstr[INET6_ADDRSTRLEN];

	inet_ntop(their_addr.ss_family, get_inaddr((struct sockaddr*)&their_addr), ipstr, INET6_ADDRSTRLEN);
	SDL_Log("Accepted connection from %s, socket: %d\n", ipstr, newfd);

	FD_SET(newfd, clsMasterClientfds);
	if (newfd > clsMaxfd) {
		clsMaxfd = newfd;
	}

	return newfd;
}

int Server::deleteConnection(int clientfd) {

	FD_CLR(clientfd, clsMasterClientfds);
	FD_CLR(clientfd, clsClientRfds);
	FD_CLR(clientfd, clsClientWfds);
	FD_CLR(clientfd, clsClientEfds);
	close(clientfd);

	return 0;
}

int Server::handleClientData(int clientfd) {

	char recv_buffer[MAXMSGSIZE];
	std::string fullmessage;

	int bytes_recv, total_bytes_recv = 0;
	do {
		if (bytes_recv = recv(clientfd, recv_buffer, MAXMSGSIZE, 0); bytes_recv < 0) {
			SDL_Log("Server::handleClientData() recv() for client socket: %d failed. Errno: %d\n", clientfd, errno);
			return -1;
		}

		if (bytes_recv == 0) { // client ended communication
			SDL_Log("Socket %d ended communication\n", clientfd);
			deleteConnection(clientfd);
			exit(3); // DEBUG
			break;
		}
		recv_buffer[bytes_recv] = '\0';
		fullmessage.append(recv_buffer);
		total_bytes_recv += bytes_recv;
	} while (bytes_recv >= MAXMSGSIZE);

	SDL_Log("Message recieved from client socket %d: %s\n", clientfd, fullmessage.c_str());
	SDL_Log("Bytes recieved: %d\n", total_bytes_recv);

	char* fullmessage_c = (char*)malloc(sizeof(char)*total_bytes_recv);

	char *sender = (char*)malloc(sizeof(char)*total_bytes_recv);
	char *senderMessage = (char*)malloc(sizeof(char)*total_bytes_recv);

	char* msgPtr = fullmessage_c;

	strncpy(fullmessage_c, fullmessage.c_str(), total_bytes_recv);

	sender = strtok_r(fullmessage_c, ",", &msgPtr);
	senderMessage = strtok_r(nullptr, ",", &msgPtr);

	BoardMessage *newBM = new BoardMessage(senderMessage, sender);
	clsBoard.addMessage(newBM);

	SDL_Log("Name: %s\nMessage: %s\n", sender, senderMessage);

	return 0;
}

int Server::sendClientMessage(int clientfd, std::string message) {

	SDL_Log("Sent client %d a message\n", clientfd); // DEBUG

	char buffer[MAXMSGSIZE];
	int bytes_stored;
	int bytes_sent;
	if (bytes_stored = snprintf(buffer, MAXMSGSIZE-1, "%s,%s", "Name", message.c_str()); bytes_stored == -1) {
		SDL_Log("Server::sendClientMessage Error during formatting\n");
		exit(1); // DEBUG
	}
	if (bytes_stored > MAXMSGSIZE-1) {
		SDL_Log("Message server is trying to send is too large\n");
		return -1;
	}

	buffer[bytes_stored] = '\0';

	if (bytes_sent = send(clientfd, buffer, MAXMSGSIZE, 0); bytes_sent == -1) {
		SDL_Log("Server::sendClientMessage send() failed. errno: %d\n", errno);
		return -1;
	}

	SDL_Log("Message sent: %s\n", buffer);
	SDL_Log("Sent %d bytes\n", bytes_sent);

	return 0;
}

int Server::handleClientError(int clientfd) {

	int err = 0;
	socklen_t errlen = sizeof(err);

	if (getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
		SDL_Log("Server::handleClientError setsockopt() failed. Exiting program. Error: %s\n", strerror(errno));
		exit(1);
	}

	if (err != 0) {
		SDL_Log("Error on socket %d: %s\n", clientfd, strerror(err));
	}

	return 0;
}

int Server::pollConnections() {
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(clsClientRfds);
	FD_ZERO(clsClientWfds);
	FD_ZERO(clsClientEfds);
	*clsClientRfds = *clsMasterClientfds;
	*clsClientWfds = *clsMasterClientfds;
	*clsClientEfds = *clsMasterClientfds;

	FD_CLR(clsListenfd, clsClientWfds);
	FD_CLR(clsListenfd, clsClientEfds);

	int socks = select(clsMaxfd+1, clsClientRfds, nullptr, nullptr, &tv);
	
	for (int i = 0; i < clsMaxfd+1; i++) {
		if (FD_ISSET(i, clsClientRfds)) {
			SDL_Log("Socket %d sent a message\n", i);
			if (i == clsListenfd) { // new client trying to connect
				SDL_Log("New connection\n");
				handleNewConnection();
			}
			else {
				handleClientData(i);
			}
			return MESSAGE_RECVD;
		}

		// if (FD_ISSET(i, clsClientWfds)) {
		// 	SDL_Log("Socket %d ready to recieve message\n", i);
		// 	std::string m = "hello";
		// 	sendClientMessage(i, m);
		// 	SDL_Log("\n");
		// 	return MESSAGE_SENT;
		// }

		// if (FD_ISSET(i, clsClientEfds)) {
		// 	handleClientError(i);
		// 	SDL_Log("\n");
		// 	return ERROR;
		// }

	}

	return NO_CHANGE;
}
