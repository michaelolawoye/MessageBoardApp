class BoardMessage {

	private:
		SDL_Surface* cSurface{nullptr};

		BoardMessage* prevMessage{nullptr};
		BoardMessage* nextMessage{nullptr};

		int cWidth, cHeight = 0;
		std::string cMessage;
		std::string cSenderName;


	public:

		void destroySurface();

		BoardMessage(std::string message, std::string sender);

		~BoardMessage();

		SDL_Surface* getSurface();

		int getWidth();
		int getHeight();

		BoardMessage* getPrevMessage();
		BoardMessage* getNextMessage();
		void setPrevMessage(BoardMessage*);
		void setNextMessage(BoardMessage*);

		std::string getMessage();
		std::string getSenderName();

		bool intializeSurface(TTF_Font*);
};


class Board {

	private:
		BoardMessage* cMessages{nullptr};
		BoardMessage* currMessage{nullptr};
		int cMessageCount = 0;
		SDL_Texture* cTextures[MAX_MESSAGES];
		SDL_Renderer* cRenderer{nullptr};
		TTF_Font* cFont{nullptr};

	public:
		bool moveToNextMessage();
		bool moveToPrevMessage();

		void destroyTextures();
		bool createTextures();

		bool addMessage(BoardMessage*);
		bool renderTextures(SDL_FRect);

		int getMessageCount();

		Board(SDL_Renderer* renderer, TTF_Font* font);

		~Board();


		// ***TEMPORARY DEBUGGING FUNCTION***
		bool listTexturesAndSurfaces();

};

