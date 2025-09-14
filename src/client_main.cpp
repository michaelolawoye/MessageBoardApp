#include "Networking_Linux_Client_Functions.cpp"


int main(int argc, char* argv[]) {

	int sock;

	if (sock = createClientSocket("Ubuntu-24", MY_PORT); sock == -1) {
		return -1;
	}

	printf("socket: %d\n", sock);

	std::string m;
	char blackhole[256];

	while (1) {
		std::getline(std::cin,m);
		if (m == "end") {
			printf("Socket closed\n");
			close(sock);
			return 0;
		}
		printf("\nmessage: %s, length: %d\n", m.c_str(), static_cast<int>(m.length()));
		if (m.length() >= 1) {
			sendMessage(sock, m);
		}
		else {
			printf("\nmessage can't be empty\n"); // DEBUG
			break;
		}
		printf("\n");
	}

	return 0;
}

