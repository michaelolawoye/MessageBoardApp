#include "../includes/Project_Includes.hpp"

int createClientSocket(std::string, int);
int sendMessage(int clientfd, std::string message);

int main(int argc, char* argv[]) {

	int sock;

	if (sock = createClientSocket("Ubuntu-24", MY_PORT); sock == -1) {
		return -1;
	}

	printf("socket: %d\n", sock);

	std::string m;

	while (1) {
		std::cin>>m;
		if (m == "end") {
			printf("Socket closed\n");
			close(sock);
			return 0;
		}
		printf("\nm: %s, length: %d\n", m.c_str(), static_cast<int>(m.length()));
		if (m.length() > 1) {
			sendMessage(sock, m);
		}
		else {
			break;
		}
	}

	return 0;
}

int createClientSocket(std::string serverip, int port) {

	struct addrinfo *myaddrinfo, hints;
	struct addrinfo *curr_addrinfo;
	int gai_error;
	int clientfd;

	char ch_port[50];
	char ipstr[INET6_ADDRSTRLEN];
	
	sprintf(ch_port, "%d", port);

	bzero(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (gai_error = getaddrinfo(serverip.c_str(), ch_port, &hints, &myaddrinfo); gai_error != 0) {
		printf("getaddrinfo() failed. Error: %d\n", gai_error);
		return -1;
	}

	for (curr_addrinfo = myaddrinfo; curr_addrinfo !=  nullptr; curr_addrinfo = curr_addrinfo->ai_next) {

		inet_ntop(curr_addrinfo->ai_family, get_inaddr(static_cast<struct sockaddr*>(curr_addrinfo->ai_addr)), ipstr, sizeof(ipstr));

		if (clientfd = socket(curr_addrinfo->ai_family, curr_addrinfo->ai_socktype, curr_addrinfo->ai_protocol); clientfd == -1) {
			printf("socket() failed for %s, trying next one. errno: %d\n", ipstr, errno);
			close(clientfd);
			continue;
		}

		printf("Client created socket for ip: %s\n", ipstr);

		if (connect(clientfd, curr_addrinfo->ai_addr, curr_addrinfo->ai_addrlen) == -1) {
			printf("connect() failed for %s, trying next ip. errno: %d\n", ipstr, errno);
			close(clientfd);
			continue;
		}

		printf("Client connected socket %d to ip: %s\n", clientfd, ipstr);

		break;
	}

	if (curr_addrinfo == nullptr) {
		printf("Socket creation/connection failed...\n");
		return -1;
	}

	printf("Socket successfully connected\n");

	freeaddrinfo(curr_addrinfo);

	return clientfd;
}


int sendMessage(int clientfd, std::string message) {

	int bytes_sent = 0;
	int message_bytes = message.length();
	int packet_bytes = 0;

	std::string curr_message = message;
	do {
		curr_message = message.substr(bytes_sent, message_bytes-bytes_sent);

		packet_bytes = send(clientfd, curr_message.c_str(), curr_message.length(), 0);
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