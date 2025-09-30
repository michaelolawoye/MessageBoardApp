#include "../includes/Project_Includes.hpp"
constexpr int MAXMSGSIZE{256};

class Client {

	private:
		double clsClientCycles;

		int clsSocket;
		int clsServerPort;
		std::string clsServerName; // can be IPv4, IPv6 or name

		int createSocket();

		Board& clsBoard;


	public:
		Client(std::string server_name, int server_port, Board& board);
		~Client();

		int sendMessage(std::string message);
		int getSocket();
		int recvMessage();
};

Client::Client(std::string server_name, int server_port, Board& board): clsServerName(server_name), clsServerPort(server_port), clsBoard(board) {

	clsClientCycles = CLIENT_TIMEOUT;
	if (createSocket() == -1) {
		exit(1);
	}
} 

Client::~Client() {
	close(clsSocket);
}

int Client::getSocket() { return clsSocket; }

int Client::createSocket() {

	struct addrinfo *myaddrinfo, hints;
	struct addrinfo *curr_addrinfo;
	int gai_error;

	char ch_port[50];
	char ipstr[INET6_ADDRSTRLEN];
	
	sprintf(ch_port, "%d", clsServerPort);

	bzero(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (gai_error = getaddrinfo(clsServerName.c_str(), ch_port, &hints, &myaddrinfo); gai_error != 0) {
		printf("getaddrinfo() failed. Error: %d\n", gai_error);
		return -1;
	}

	for (curr_addrinfo = myaddrinfo; curr_addrinfo !=  nullptr; curr_addrinfo = curr_addrinfo->ai_next) {

		inet_ntop(curr_addrinfo->ai_family, get_inaddr(static_cast<struct sockaddr*>(curr_addrinfo->ai_addr)), ipstr, sizeof(ipstr));

		if (clsSocket = socket(curr_addrinfo->ai_family, curr_addrinfo->ai_socktype, curr_addrinfo->ai_protocol); clsSocket == -1) {
			printf("socket() failed for %s, trying next one. errno: %d\n", ipstr, errno);
			close(clsSocket);
			continue;
		}

		printf("Client created socket for ip: %s\n", ipstr);

		if (connect(clsSocket, curr_addrinfo->ai_addr, curr_addrinfo->ai_addrlen) == -1) {
			printf("connect() failed for %s, trying next ip. errno: %d\n", ipstr, errno);
			close(clsSocket);
			continue;
		}

		printf("Client connected socket %d to ip: %s\n", clsSocket, ipstr);

		break;
	}

	if (curr_addrinfo == nullptr) {
		printf("Socket creation/connection failed...\n");
		return -1;
	}

	printf("Socket successfully connected\n");

	freeaddrinfo(curr_addrinfo);

	return clsSocket;
}

int Client::sendMessage(std::string message) {

	std::string deviceName = getDeviceName();
	std::string comma = ",";
	std::string fullmessage = deviceName+comma+message;
	int bytes_sent = 0;
	int message_bytes = fullmessage.length();
	int packet_bytes = 0;

	std::string curr_message = fullmessage;
	do {
		curr_message = fullmessage.substr(bytes_sent, message_bytes-bytes_sent);
		packet_bytes = send(clsSocket, curr_message.c_str(), curr_message.length(), 0);
		printf("Bytes sent: %d\n", packet_bytes);
		fflush(stdout);

		if (packet_bytes == -1) {
			printf("Couldn't send  message. errno: %d\n", errno);
			return -1;
		}

		bytes_sent += packet_bytes;
	} while (bytes_sent < message_bytes);

	return 0;
}

int Client::recvMessage() {

	char recv_buffer[MAXMSGSIZE];
	std::string fullmessage;

	int bytes_recv, total_bytes_recv = 0;
	do {
		if (bytes_recv = recv(clsSocket, recv_buffer, MAXMSGSIZE, MSG_DONTWAIT); bytes_recv < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return 0;
			}
			SDL_Log("Client::recvMessage() failed. Errno: %d\n", errno);
			return -1;
		}

		if (bytes_recv == 0) { // server ended communication
			SDL_Log("Socket %d ended communication\n", clsSocket);
			exit(3); // DEBUG
			break;
		}
		recv_buffer[bytes_recv] = '\0';
		fullmessage.append(recv_buffer);
		total_bytes_recv += bytes_recv;
	} while (bytes_recv >= MAXMSGSIZE);

	char* fullmessage_c = (char*)malloc(sizeof(char)*total_bytes_recv);
	strncpy(fullmessage_c, fullmessage.c_str(), fullmessage.length());
	char* msgPtr = fullmessage_c;

	char *sender = (char*)malloc(sizeof(char)*total_bytes_recv);
	char *senderMessage = (char*)malloc(sizeof(char)*total_bytes_recv);

	char* temp = (char*)malloc(sizeof(char)*total_bytes_recv);
	SDL_Log("fullmessage: %s\n", fullmessage.c_str()); // DEBUG
	char* type = strtok_r(fullmessage_c, ",", &msgPtr);
	type[1] = '\0';
	SDL_Log("type: %s\n", type);

	// message recieved structure: 
	// m,sendername,sendermessage
	// b,sendername1,sendermessage1;sendername2,sendermessage2;
	if (type[0] == 'm') { // recieved a single message
		sender = strtok_r(nullptr, ",", &msgPtr);
		senderMessage = strtok_r(nullptr, ";", &msgPtr);

		BoardMessage* newBM = new BoardMessage(senderMessage, sender);	
		clsBoard.addMessage(newBM);
	}
	else if (type[0] == 'b') { // recieved a new board of messages
		clsBoard.destroyMessages();
		char* msgPtr2 = temp;
		while ((temp = strtok_r(nullptr, ";", &msgPtr)) != nullptr) {
			sender = strtok_r(temp, ",", &msgPtr2);
			senderMessage = strtok_r(nullptr, ";", &msgPtr2);

			BoardMessage* newBM = new BoardMessage(senderMessage, sender);
			clsBoard.addMessage(newBM);
		}
	}
	else {
		SDL_Log("Message got messed up.\n");
		return -1;
	}


	return 1;
}
