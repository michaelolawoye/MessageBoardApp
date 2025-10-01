#ifndef NETWORKING_LINUX_CLASS_DECLARATIONS_HPP
#define NETWORKING_LINUX_CLASS_DECLARATIONS_HPP

class Server{

	private:
		int clsListenfd;
		int clsPort;
		fd_set* clsMasterClientfds; // master fd_set to keep track of all fds

		// fd_sets for select() arguments
		fd_set* clsClientRfds;
		fd_set* clsClientWfds;
		fd_set* clsClientEfds;

		int clsMaxfd; // value for select()'s first argument

		Board& clsBoard;

		std::unordered_map<int, std::queue<std::string>> clsClientMsgQueues; // keeps track of each client's queue of unrecieved messages

		void* get_inaddr(struct sockaddr*);
		int createListenSocket();
		int handleNewConnection();
		int deleteConnection(int clientfd);

		int handleClientData(int clientfd);
		int sendClientMessage(int clientfd);
		int handleClientError(int clientfd);


	public:
		Server(int port, Board& board);
		~Server();

		int addMessageToQueues(std::string sender, std::string message);
		int pollConnections();

		int getListenFd();
		fd_set* getMasterFds();
};

#endif
