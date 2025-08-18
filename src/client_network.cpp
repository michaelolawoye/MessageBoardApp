#include "../includes/Project_Includes.hpp"

int createClientSocket(std::string, int);
int sendMessage(int clientfd, std::string message);

int main(int argc, char* argv[]) {


	int port = 0;

	int sock = createClientSocket("tcpbin.com", 4242);
	printf("socket: %d\n", sock);

	std::string m;
	std::cin >> m;

	sendMessage(sock, m);

	return 0;
}

int createClientSocket(std::string serverip, int port) {

	struct addrinfo *myaddrinfo, hints;
	struct addrinfo *curr_addrinfo;
	int gai_error;
	int clientfd;

	char ch_port[50];
	sprintf(ch_port, "%d", port);

	bzero(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (gai_error = getaddrinfo(serverip.c_str(), ch_port, &hints, &myaddrinfo); gai_error != 0) {
		printf("getaddrinfo() failed. Error: %d\n", gai_error);
		return -1;
	}

	for (curr_addrinfo = myaddrinfo; curr_addrinfo !=  nullptr; curr_addrinfo = curr_addrinfo->ai_next) {

		char ipstr[INET6_ADDRSTRLEN];

		inet_ntop(curr_addrinfo->ai_family, get_inaddr(static_cast<struct sockaddr*>(curr_addrinfo->ai_addr)), ipstr, sizeof(ipstr));

		if (clientfd = socket(curr_addrinfo->ai_family, curr_addrinfo->ai_socktype, curr_addrinfo->ai_protocol); clientfd == -1) {
			printf("socket() failed for %s, trying next one. errno: %d\n", ipstr, errno);
			continue;
		}

		printf("Client created socket for ip: %s\n", ipstr);

		if (connect(clientfd, curr_addrinfo->ai_addr, curr_addrinfo->ai_addrlen) == -1) {
			printf("connect() failed for %s, trying next ip. errno: %d\n", ipstr, errno);
			continue;
		}

		break;
	}

	if (clientfd == -1) {
		printf("Socket creation failed...\n");
		return -1;
	}

	printf("Socket successfully connected\n");



	return clientfd;
}


int sendMessage(int clientfd, std::string message) {

	int bytes_sent = 0;
	int message_bytes = message.length();
	int packet_bytes = 0;

	std::string curr_message = message;

	while (bytes_sent < message_bytes) {

		curr_message = message.substr(bytes_sent, message_bytes-bytes_sent);

		packet_bytes = send(clientfd, curr_message.c_str(), curr_message.length(), 0);

		if (packet_bytes == -1) {
			printf("Couldn't send  message. errno: %d\n", errno);
			return -1;
		}

		bytes_sent += packet_bytes;
	}

	char returnMessage[1024] = "nothing";
	int bytes_recv = -100;

	if ((bytes_recv = recv(clientfd, returnMessage, 1024, 0)) == -1) {
		printf("recv() failed. errno: %d", errno);
	}
	printf("bytes recieved: %d\n", bytes_recv);
	printf("returnMessage: %s\n", returnMessage);

	printf("finished\n");
	return 0;
}