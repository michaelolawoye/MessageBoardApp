#include "../includes/SDL_Includes.h"
#include "../includes/SDL_Constants.h"
#include "../includes/SDL_Declarations.h"


class LTexture {

	public:
		LTexture(SDL_Renderer** renderer): cRenderer{renderer}, width{0}, height{0} {}

		~LTexture() { destroy(); }

		bool loadFromFile(std::string path);

		void destroy() {
			SDL_DestroyTexture(cTexture);
			cTexture = nullptr;

			width = 0;
			height = 0;
		}

		void render(float x, float y);

		int getWidth() { return width; }
		int getHeight() { return height; }
		bool isLoaded() { return cTexture != nullptr; }

	private:
		SDL_Texture* cTexture;
		SDL_Renderer** cRenderer;

		int width;
		int height;
};

bool LTexture::loadFromFile(std::string path) {

	// clean up texture if already exists
	destroy();

	SDL_Surface* surface = nullptr;

	if (surface = IMG_Load(path.c_str()); surface == nullptr) {
		SDL_Log("Couldn't create surface for the texture. SDL_Error:%s\n", SDL_GetError());
		return false;
	}

	if (cTexture = SDL_CreateTextureFromSurface(*cRenderer, surface); cTexture == nullptr) {
		SDL_Log("Couldn't create texture. SDL_Error:%s\n", SDL_GetError());
		return false;
	}

	width = surface->w;
	height = surface->h;

	SDL_DestroySurface(surface);

	return true;
}

void LTexture::render(float x, float y) {

	SDL_FRect dstRect{x, y, static_cast<float>(width), static_cast<float>(height)};

	SDL_RenderTexture(*cRenderer, cTexture, nullptr, &dstRect);
}

int main(int argc, char* argv[]) {

	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr; 
	SDL_Surface* mSurface = nullptr;
	LTexture texture {&mRenderer};

	if (!init(&mWindow, &mRenderer, &mSurface)) {
		return 1;
	}

	std::string path {"./assets/simplebmp.bmp"};
	if (!loadMedia(&texture, path)) {
		return 1;
	}


	bool quit = false;
	SDL_Event e;
	SDL_zero(e);

	while (!quit) {

		while (SDL_PollEvent(&e)) {

			switch (e.type) {
				case SDL_EVENT_QUIT:
					quit = true;
					break;

				case SDL_EVENT_KEY_DOWN:

					switch (e.key.key) {
						case SDLK_UP:
							SDL_Log("Up\n");
							SDL_SetRenderDrawColor(mRenderer, 0, 0, 0xFF, 0xFF);
							break;
						case SDLK_DOWN:
							SDL_Log("Down\n");
							SDL_SetRenderDrawColor(mRenderer, 0xFF, 0, 0, 0xFF);
							break;
						case SDLK_LEFT:
							SDL_Log("Left\n");
							SDL_SetRenderDrawColor(mRenderer, 0, 0xFF, 0, 0xFF);
							break;
						case SDLK_RIGHT:
							SDL_Log("Right\n");
							SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0, 0xFF);
							break;
						default:
							break;
					}

				default:
					break;
			}
		}

		SDL_RenderClear(mRenderer);

		texture.render(0.f, 0.f);
		SDL_RenderPresent(mRenderer);
	}

	close(&mWindow, &mRenderer, &mSurface);


	return 0;
}


bool init(SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** background) {

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL! SDL_Error: %s", SDL_GetError());
		return false;
	}

	if (!SDL_CreateWindowAndRenderer("SDL3 Window", SCREEN_WIDTH, SCREEN_HEIGHT, 0, window, renderer)) {
		SDL_Log("Couldn't create main window! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if (*background = SDL_GetWindowSurface(*window); *background == nullptr) {
		SDL_Log("Couldn't create main surface! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}


bool loadMedia(void* texture, std::string path) {

	if (!static_cast<LTexture*>(texture)->loadFromFile(path)) {
		return false;
	}

	return true;
}

void close (SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** surface) {

	SDL_DestroySurface(*surface);
	*surface = nullptr;

	SDL_DestroyRenderer(*renderer);
	renderer = nullptr;

	SDL_DestroyWindow(*window);
	window = nullptr;

	SDL_Quit();	
}
