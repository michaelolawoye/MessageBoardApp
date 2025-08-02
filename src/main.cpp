#include "../includes/SDL_Includes.h"
#include "../includes/SDL_Constants.h"
#include "../includes/SDL_Declarations.h"

class BoardMessage {

	private:
		SDL_Surface* cSurface{nullptr};
		TTF_Font* cFont{nullptr};
		int cWidth, cHeight = 0;
		std::string cMessage = 0;
		std::string cSenderName;

	public:

		void destroySurface();

		BoardMessage(std::string message, std::string sender, TTF_Font* font):
		cMessage(message), cSenderName(sender), cFont(font) {
			SDL_Log("Surface initialized.");
		}

		~BoardMessage() {
			destroySurface();
			SDL_Log("Surface destroyed");
		}

		int getWidth() { return cWidth; }
		int getHeight() { return cHeight; }
		std::string getMessage() { return cMessage; }
		std::string getSenderName() { return cSenderName; }
		bool surfaceLoaded() { return cSurface!=nullptr; }

		bool intializeSurface();
};

bool BoardMessage::intializeSurface() {

	if (cSurface) {
		SDL_Log("Surface already initialized!.");
		return false;
	}

	SDL_Color color = {0, 0, 0, 0};
	if (cSurface = TTF_RenderText_Blended(cFont, getMessage().c_str(), 0, color); cSurface == nullptr) {
		SDL_Log("Couldn't initialize surface. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	cWidth = cSurface->w;
	cHeight = cSurface->h;

	return true;	
}

void BoardMessage::destroySurface() {

	if (!cSurface) {
		SDL_Log("Surface is already destroyed\n");
		return;
	}

	SDL_DestroySurface(cSurface);
	cSurface = nullptr;
	cWidth = 0;
	cHeight = 0;
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
	
	BoardMessage bm{"hello", "my device", font};

	bm.intializeSurface();

	SDL_Log("BoardMessage: %s\n", bm.getMessage().c_str());

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
