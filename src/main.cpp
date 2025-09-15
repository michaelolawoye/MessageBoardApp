#include "SDL_Class_Functions.cpp"
#include "Networking_Linux_Server_Functions.cpp"


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
				new_render = true;
				break;
			case MESSAGE_SEND:
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


