#include "../includes/SDL_Includes.h"
#include "../includes/SDL_Constants.h"
#include "../includes/SDL_Declarations.h"

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

		BoardMessage(std::string message, std::string sender):
		cMessage(message), cSenderName(sender) {
			SDL_Log("Message Surface: %s initialized.\n", cMessage.c_str());
		}

		~BoardMessage() {
			destroySurface();
			SDL_Log("Message Surface: %s destroyed\n", cMessage.c_str());
		}

		SDL_Surface* getSurface() { return cSurface; }

		int getWidth() { return cWidth; }
		int getHeight() { return cHeight; }

		BoardMessage* getPrevMessage() { return prevMessage; }
		BoardMessage* getNextMessage() { return nextMessage; }
		void setPrevMessage(BoardMessage* m) { prevMessage = m; }
		void setNextMessage(BoardMessage* m) { nextMessage = m; }

		std::string getMessage() { return cMessage; }
		std::string getSenderName() { return cSenderName; }

		bool surfaceLoaded() { return cSurface!=nullptr; }

		bool intializeSurface(TTF_Font* font);
};

bool BoardMessage::intializeSurface(TTF_Font* font) {

	if (cSurface) {
		SDL_Log("Surface already initialized!.");
		return false;
	}

	SDL_Color color = {0, 0, 0, 0};
	if (cSurface = TTF_RenderText_Blended_Wrapped(font, getMessage().c_str(), 0, color, 700); cSurface == nullptr) {
		SDL_Log("Couldn't initialize surface. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	cWidth = cSurface->w;
	cHeight = cSurface->h;

	return true;	
}

void BoardMessage::destroySurface() {

	if (!cSurface) {
		SDL_Log("Surface: %s is not initialized\n", cMessage.c_str());
		return;
	}

	SDL_DestroySurface(cSurface);
	cSurface = nullptr;
	cWidth = 0;
	cHeight = 0;
}


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

		bool addMessage(BoardMessage* message);
		bool createTextures();
		bool renderTextures(SDL_FRect dstSizeRect);

		int getMessageCount() { return cMessageCount; }

		Board(SDL_Renderer* renderer, TTF_Font* font): cRenderer(renderer), cFont(font) {}

		~Board() {
			destroyTextures();
		}


		// ***TEMPORARY DEBUGGING FUNCTION***
		bool listTexturesAndSurfaces();

};

bool Board::listTexturesAndSurfaces() {

	SDL_Log("\n------Board Surfaces and Textures------\n\n");
	currMessage = cMessages;

	for (int x = 1; currMessage != nullptr; currMessage = currMessage->getNextMessage(), x++) {
		SDL_Log("Board message %d -> Message: %s, Device: %s\n", x, currMessage->getMessage().c_str(), currMessage->getSenderName().c_str());
	}
	SDL_Log("\n");
	int i = 1;

	for (auto& texture: cTextures) {
		SDL_Log("Texture %d is initialized: %d\n", i, texture!=nullptr);
		i++;
	}

	SDL_Log("\n-----End of board Surfaces and Textures------\n\n");
	return true;
}

bool Board::moveToNextMessage() {

	if (!cMessages) {
		SDL_Log("No messages on board, can't move to next message.\n");
		return false;
	}

	if (!currMessage->getNextMessage()) {
		return false;
	}

	currMessage = currMessage->getNextMessage();
	return true;
}

bool Board::moveToPrevMessage() {

	if (!cMessages) {
		SDL_Log("No messages on board, can't move to previous message\n");
		return false;
	}

	if (!currMessage->getPrevMessage()) {
		return false;
	}

	currMessage = currMessage->getPrevMessage();
	return true;
}

void Board::destroyTextures() {

	for (auto& texture: cTextures) {
		if (!texture)
			continue;
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
}

bool Board::addMessage(BoardMessage* message) {

	if (!message) {
		SDL_Log("Can't add null message to board\n");
		return false;
	}

	if (cMessageCount >= MAX_MESSAGES) {
		SDL_Log("Can't add more messages, max message size reached.\n");
	}

	if (!cMessages) {
		cMessages = message;
		currMessage = cMessages;
		cMessageCount++;
		SDL_Log("First message added to board. Number of messages: %d\n", cMessageCount);
		return true;
	}

	while (moveToNextMessage()) {}

	currMessage->setNextMessage(message);
	message->setPrevMessage(currMessage);
	message->setNextMessage(nullptr);
	cMessageCount++;

	currMessage = cMessages;
	SDL_Log("Message added to board. Number of messages: %d\n", cMessageCount);

	return true;
}

bool Board::createTextures() {

	if (!cMessages) {
		SDL_Log("No messages on board to create textures\n");
		return false;
	}

	for (currMessage = cMessages; currMessage != nullptr; currMessage = currMessage->getNextMessage()) {
		currMessage->intializeSurface(cFont);
	}

	currMessage = cMessages;

	for (int i = 0; currMessage != nullptr && i <= MAX_MESSAGES-1; currMessage = currMessage->getNextMessage(), i++) {
		if (cTextures[i] = SDL_CreateTextureFromSurface(cRenderer, currMessage->getSurface()); cTextures[i] == nullptr) {
			SDL_Log("cTextures[%d] is not initialized properly something went wrong... SDL_Error: %s\n", i, SDL_GetError());
		}
	}

	return true;
}

bool Board::renderTextures(SDL_FRect dstSizeRect) {

	float x = 0, y = 0;

	for (auto& texture: cTextures) {
		if (!texture) {
			continue;
		}
		dstSizeRect.x = x;
		dstSizeRect.y = y;
		SDL_RenderTexture(cRenderer, texture, nullptr, &dstSizeRect);

		y += dstSizeRect.h;

		if (y > SCREEN_HEIGHT-dstSizeRect.h) {
			y = 0;
			x += dstSizeRect.w;
		}

		if (x > SCREEN_WIDTH)
			return false;
	}
	SDL_RenderPresent(cRenderer);
	return true;
}


int main(int argc, char* argv[]) {

	SDL_Window* mWindow{nullptr};
	SDL_Renderer* mRenderer{nullptr};
	SDL_Surface* mSurface{nullptr};

	if (!init(mWindow, mRenderer, mSurface)) {
		return 1;
	}

	std::string titleFontPath{"./assets/Roboto/static/Roboto-Black.ttf"};
	TTF_Font* font{nullptr};
	if (font = TTF_OpenFont(titleFontPath.c_str(), titleFontPath.length()); font == nullptr) {
		SDL_Log("Couldn't load font. SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	
	Board b{mRenderer, font};
	BoardMessage bm1{"bm1Message", "my device1"};
	BoardMessage bm2{"Hello my name is Michael, I'm making this program to get better at application programming", "my device2"};
	BoardMessage bm3{"bm3Message", "my device3"};
	BoardMessage bm4{"bm4Message", "my device4"};
	BoardMessage bm5{"bm5Message", "my device5"};
	BoardMessage bm6{"bm6Message", "my device6"};
	BoardMessage bm7{"bm7Message", "my device7"};
	BoardMessage bm8{"bm8Message", "my device8"};
	BoardMessage bm9{"bm9Message", "my device9"};
	BoardMessage bm10{"bm10Message", "my device10"};

	b.addMessage(&bm1);
	b.addMessage(&bm2);
	b.addMessage(&bm3);
	b.addMessage(&bm4);
	b.addMessage(&bm5);
	b.addMessage(&bm6);
	b.addMessage(&bm7);
	b.addMessage(&bm8);
	b.addMessage(&bm9);
	b.addMessage(&bm10);
	b.createTextures();
	b.listTexturesAndSurfaces();

	SDL_FRect dstRect{0, 0, SCREEN_WIDTH/3, SCREEN_HEIGHT/8};

	bool quit = false;
	SDL_Event e;
	SDL_zero(e);

	while (!quit) {
		while (SDL_PollEvent(&e)) {

			switch(e.type) {
				case SDL_EVENT_QUIT:
					quit = true;
					break;

			}
		}

		SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(mRenderer);

		b.renderTextures(dstRect);
		SDL_RenderPresent(mRenderer);
	}
	close(mWindow, mRenderer, mSurface);

	return 0;
}


bool init(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Surface*& surface) {

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if (!SDL_CreateWindowAndRenderer("Message Board", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window and/or renderer. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if (surface = SDL_GetWindowSurface(window); surface == nullptr) {
		SDL_Log("Couldn't get window surface. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if (!TTF_Init()) {
		SDL_Log("Couldn't initialize TTF. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void close(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Surface*& surface) {

	SDL_DestroySurface(surface);
	surface = nullptr;

	SDL_DestroyRenderer(renderer);
	renderer = nullptr;

	SDL_DestroyWindow(window);
	window = nullptr;

	TTF_Quit();
	SDL_Quit();	
}


std::string getDeviceName() {

	char name[256];

	#ifdef _WIN32
		DWORD size = 256
		GetComputerNameA(name, &size);

	#else
		gethostname(name, sizeof(name));
	#endif

	return static_cast<std::string>(name);
}
