#include "../includes/Project_Includes.hpp"

// BoardMessage methods

// Constructor and Destructor
BoardMessage::BoardMessage(std::string message, std::string sender):
	cMessage(message), cSenderName(sender) {
		SDL_Log("Message Surface: \"%s\" initialized.\n", cMessage.c_str());
}

BoardMessage::~BoardMessage() {
	destroySurface();
	SDL_Log("Message class instance: \"%s\" destroyed\n", cMessage.c_str());
}

// Accessor methods
SDL_Surface* BoardMessage::getSurface() { return cSurface; }

int BoardMessage::getWidth() { return cWidth; }
int BoardMessage::getHeight() { return cHeight; }

BoardMessage* BoardMessage::getPrevMessage() { return prevMessage; }
BoardMessage* BoardMessage::getNextMessage() { return nextMessage; }

std::string BoardMessage::getMessage() { return cMessage; }
std::string BoardMessage::getSenderName() { return cSenderName; }

// Mutator methods
void BoardMessage::setPrevMessage(BoardMessage* m) { prevMessage = m; }
void BoardMessage::setNextMessage(BoardMessage* m) { nextMessage = m; }

void BoardMessage::changeMessage(std::string newMessage) { cMessage = newMessage; }

// Surface methods
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


// Board methods

// Constructor and Destructor
Board::Board(SDL_Renderer* renderer, TTF_Font* font): cRenderer(renderer), cFont(font) {
	SDL_zero(cTextures);
}

Board::~Board() {
	destroyTextures();
	destroyMessages();
}

// Debug method
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

// Message methods
int Board::getMessageCount() { return cMessageCount; }
BoardMessage& Board::getCurrMessage() { return currMessage; }

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

// Texture methods
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

void Board::destroyTextures() {

	for (auto& texture: cTextures) {
		if (!texture)
			continue;
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
}
