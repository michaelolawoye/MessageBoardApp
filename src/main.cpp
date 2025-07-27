#include "../includes/SDL_Includes.h"
#include "../includes/SDL_Constants.h"
#include "../includes/SDL_Declarations.h"


class LTexture {

	public:

		static constexpr float originalSize{-1.f};

		LTexture(SDL_Renderer** renderer): cRenderer{renderer}, cWidth{0}, cHeight{0} {}

		~LTexture() { destroy(); }

		bool loadImage(std::string path);
		bool loadText(std::string text, SDL_Color color, TTF_Font* font);

		void destroy() {
			SDL_DestroyTexture(cTexture);
			cTexture = nullptr;

			cWidth = 0;
			cHeight = 0;
		}

		void render(SDL_FRect* srcRegion, SDL_FRect* dstRegion);

		int getWidth() { return cWidth; }
		int getHeight() { return cHeight; }
		bool isLoaded() { return cTexture != nullptr; }

	private:
		SDL_Texture* cTexture;
		SDL_Renderer** cRenderer;

		int cWidth;
		int cHeight;

};

bool LTexture::loadImage(std::string path) {

	// clean up texture if already exists
	destroy();

	SDL_Surface* surface = nullptr;

	if (surface = IMG_Load(path.c_str()); surface == nullptr) {
		SDL_Log("Couldn't create surface for the texture. SDL_Error:%s\n", SDL_GetError());
		return false;
	}

	// sets colour specified to be transparent on surface given
	// if (!SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 0, 0, 0xFF))) {

	// 	SDL_Log("Couldn't set color key. SDL_Error: %s\n", SDL_GetError());
	// }

	if (cTexture = SDL_CreateTextureFromSurface(*cRenderer, surface); cTexture == nullptr) {
		SDL_Log("Couldn't create texture. SDL_Error:%s\n", SDL_GetError());
		return false;
	}

	cWidth = surface->w;
	cHeight = surface->h;

	SDL_DestroySurface(surface);

	return true;
}

bool LTexture::loadText(std::string text, SDL_Color color, TTF_Font* font) {

	destroy();

	SDL_Surface* textSurface{nullptr};

	if (textSurface = TTF_RenderText_Blended(font, text.c_str(), 0, color); textSurface == nullptr) {
		SDL_Log("Couldn't create text surface. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if (cTexture = SDL_CreateTextureFromSurface(*cRenderer, textSurface); cTexture == nullptr) {
		SDL_Log("Couldn't create texture. SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	cWidth = textSurface->w;
	cHeight = textSurface->h;

	SDL_DestroySurface(textSurface);

	return true;
}

// srcRegion determines position and size of region cut out of the texture, dstRegion determines the position and size of the srcRegion placed on the screen 
void LTexture::render(SDL_FRect* srcRegion, SDL_FRect* dstRegion) {

	if (dstRegion->w <= 0 && !srcRegion) {
		dstRegion->w = cWidth;
	} else if (dstRegion->w <= 0) {
		dstRegion->w = srcRegion->w;
	} 

	if (dstRegion->h <= 0 && !srcRegion) {
		dstRegion->h = cHeight;
	} else if (dstRegion->w <= 0) {
		dstRegion->h = srcRegion->h;
	}

	// 2nd argument is the texture being cut out of by srcRegion
	SDL_RenderTexture(*cRenderer, cTexture, srcRegion, dstRegion);
}


int main(int argc, char* argv[]) {

	SDL_Window* mWindow{nullptr};
	SDL_Renderer* mRenderer{nullptr}; 
	SDL_Surface* mSurface{nullptr};

	LTexture imgTexture{&mRenderer};
	LTexture textTexture{&mRenderer};

	TTF_Font* mFont{nullptr};
	SDL_Color blackColor{0, 0, 0, 0xFF};

	std::string fontpath{"./assets/Roboto/static/Roboto-Black.ttf"};

	if (!init(&mWindow, &mRenderer, &mSurface)) {
		return 1;
	}

	if (mFont = TTF_OpenFont(fontpath.c_str(), fontpath.length()); mFont == nullptr) {
		SDL_Log("Couldn't open font: %s. SDL_Error: %s\n", fontpath.c_str(), SDL_GetError());
		return 1;
	}

	std::string sentence{"Hello World"};
	if (!textTexture.loadText(sentence, blackColor, mFont)) {
		return 1;
	}

	std::string path {"./assets/simplebmp.bmp"};
	if (!imgTexture.loadImage(path)) {
		return 1;
	}

	bool quit = false;
	SDL_Event e;
	SDL_zero(e);

	constexpr int SIMPLEBMP_WIDTH{200};
	constexpr int SIMPLEBMP_HEIGHT{200};
	constexpr int SPEED{10};
	int texture_x = 0;
	int texture_y = 0;

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

							if (texture_y > 0) { texture_y-=SPEED; }

							break;
						case SDLK_DOWN:
							SDL_Log("Down\n");

							if (texture_y <= SCREEN_HEIGHT-SIMPLEBMP_HEIGHT-SPEED) { texture_y+=SPEED; }

							break;
						case SDLK_LEFT:
							SDL_Log("Left\n");

							if (texture_x > 0) { texture_x-=SPEED; }

							break;
						case SDLK_RIGHT:
							SDL_Log("Right\n");

							if (texture_x <= SCREEN_WIDTH-SIMPLEBMP_WIDTH-SPEED) { texture_x+=SPEED; }

							break;
						default:
							break;
					}

				default:
					break;
			}
		}

		SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(mRenderer);

		SDL_FRect spriteClip{0.f, 0.f, 200.f, 200.f};

		SDL_FRect dstRect1{texture_x, texture_y, 0, 0};
		SDL_FRect dstRect2{300, 100, 200, 50};

		imgTexture.render(nullptr, &dstRect1);
		imgTexture.render(&spriteClip, &dstRect2);

		textTexture.render(nullptr, &dstRect1);
		SDL_RenderPresent(mRenderer);
	}

	TTF_CloseFont(mFont);
	mFont = nullptr;

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

	if (!TTF_Init()) {
		SDL_Log("Couldn't initialize TTF! SDL_Error: %s\n", SDL_GetError());
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

	TTF_Quit();
	SDL_Quit();	
}
