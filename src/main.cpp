#include "../includes/Project_Includes.hpp"
#include "server_network.cpp"


BoardMessage::BoardMessage(std::string message, std::string sender):
	cMessage(message), cSenderName(sender) {
		SDL_Log("Message Surface: \"%s\" initialized.\n", cMessage.c_str());
}

BoardMessage::~BoardMessage() {
	destroySurface();
	SDL_Log("Message class instance: \"%s\" destroyed\n", cMessage.c_str());
}

SDL_Surface* BoardMessage::getSurface() { return cSurface; }

int BoardMessage::getWidth() { return cWidth; }
int BoardMessage::getHeight() { return cHeight; }

BoardMessage* BoardMessage::getPrevMessage() { return prevMessage; }
BoardMessage* BoardMessage::getNextMessage() { return nextMessage; }
void BoardMessage::setPrevMessage(BoardMessage* m) { prevMessage = m; }
void BoardMessage::setNextMessage(BoardMessage* m) { nextMessage = m; }

std::string BoardMessage::getMessage() { return cMessage; }
std::string BoardMessage::getSenderName() { return cSenderName; }

void BoardMessage::changeMessage(std::string newMessage) { cMessage = newMessage; }

bool BoardMessage::intializeSurface(TTF_Font* font) {

	SDL_Color color = {0, 0, 0, 0};
	if (cSurface = TTF_RenderText_Blended_Wrapped(font, getMessage().c_str(), 0, color, TEXT_WRAP_WIDTH); cSurface == nullptr) {
		SDL_Log("Couldn't initialize surface. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	cWidth = cSurface->w;
	cHeight = cSurface->h;

	return true;	
}

void BoardMessage::destroySurface() {

	if (!cSurface) {
		SDL_Log("Surface: %s cannot be destroyed because it is not initialized\n", cMessage.c_str());
		return;
	}

	SDL_DestroySurface(cSurface);
	cSurface = nullptr;
	cWidth = 0;
	cHeight = 0;
}


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

	SDL_Log("\n-----End of Board Surfaces and Textures------\n\n");

	currMessage = cMessages;
	return true;
}

Board::Board(SDL_Renderer* renderer, TTF_Font* font): cRenderer(renderer), cFont(font) {
	SDL_zero(cTextures);
}

Board::~Board() {
	destroyTextures();
	destroyMessages();
}

int Board::getMessageCount() { return cMessageCount; }

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
		return false;
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

	currMessage = cMessages;
	return true;
}

bool Board::renderTextures(SDL_FRect dstSizeRect) {

	float x = 0, y = 0;

	int skipped_messages = (cMessageCount - MAX_RENDER_MESSAGES > 0) ? cMessageCount - MAX_RENDER_MESSAGES : 0;
	for (auto& texture: cTextures) {
		if (skipped_messages > 0) {
			skipped_messages--;
			continue;
		}
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

bool Board::destroyMessages() {

	if (!cMessages) {
		return true;
	}
	currMessage = cMessages;
	BoardMessage* nextMessage = currMessage->getNextMessage();
	
	while (nextMessage != nullptr) {
		if (!currMessage) {
			continue;
		}
		delete currMessage;
		currMessage = nextMessage;
		nextMessage = nextMessage->getNextMessage();
	}
	delete currMessage;
	currMessage = nullptr;

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
	
	Board boa{mRenderer, font};
	Board &b = boa;
	Server mServer{MY_PORT, b};


	b.createTextures();
	b.listTexturesAndSurfaces();


	SDL_FRect dstRect{0, 0, SCREEN_WIDTH/3, SCREEN_HEIGHT/8};

	bool quit = false;
	bool new_render = true;
	SDL_Event e;
	SDL_zero(e);
	std::string potentialMessage{""};

	while (!quit) {
		switch (mServer.pollConnections()) {
			case MESSAGE_RECVD:
				break;
			case MESSAGE_SENT:
				break;
			default:
				break;
		}
		while (SDL_PollEvent(&e)) {
			switch(e.type) {
				case SDL_EVENT_QUIT:
					quit = true;
					break;


				case SDL_EVENT_KEY_DOWN:
					if (handleKeyDown(mWindow, &(e.key), potentialMessage)) {
						BoardMessage* bm = new BoardMessage(potentialMessage, getDeviceName());
						b.addMessage(bm);
						potentialMessage.erase();

						new_render = true;
					}
					break;

				case SDL_EVENT_TEXT_INPUT:
					handleTextInput(mWindow, &(e.text), potentialMessage);
					break;

				default:
					break;

			}
		}

		if (new_render) {
			b.createTextures();
			new_render = false;
		}

		SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(mRenderer);

		b.renderTextures(dstRect);
		SDL_RenderPresent(mRenderer);
	}
	close(mWindow, mRenderer, mSurface);

	return 0;
}


bool handleKeyDown(SDL_Window* window, SDL_KeyboardEvent* event, std::string& potentialMessage) {

	switch (event->key) {
		case SDLK_C:
			if (SDL_GetModState() & SDL_KMOD_CTRL) {
				SDL_Log("copy\n");
				SDL_SetClipboardText(potentialMessage.c_str());
			}
			break;

		case SDLK_V:
			if (SDL_GetModState() & SDL_KMOD_CTRL) {
				SDL_Log("paste\n");
				potentialMessage = static_cast<std::string>(SDL_GetClipboardText());
			}
			break;

		case SDLK_BACKSPACE:
			if (potentialMessage.length() > 0) {
				potentialMessage.pop_back();
				SDL_Log("%s\n", potentialMessage.c_str());
			}
			break;

		case SDLK_RETURN:
			if (potentialMessage.length() > 0) {
				SDL_Log("Enter pressed\n");
				return true;
			}
			break;

		default:
			break;

	}

	return false;
}


std::string handleTextInput(SDL_Window* window, SDL_TextInputEvent* event, std::string& potentialMessage) {

	potentialMessage.append(event->text);
	SDL_Log("%s\n", potentialMessage.c_str());
	return event->text;	
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

	if (!SDL_StartTextInput(window)) {
		SDL_Log("Couldn't start text input. SDL_Error: %s\n", SDL_GetError());
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

	SDL_StopTextInput(window);
	TTF_Quit();
	SDL_Quit();	
}


