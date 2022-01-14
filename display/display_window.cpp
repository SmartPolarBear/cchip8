//
// Created by cleve on 1/13/2022.
//

#include <stdexcept>

#include "display/display_window.h"

using namespace std;

using namespace cchip8::display;

cchip8::display::display_window::display_window(int32_t w,
		uint32_t wscale,
		int32_t h,
		uint32_t hscale,
		std::string_view title)
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		throw runtime_error{ SDL_GetError() };
	}

	// initialize the window
	{
		auto window = SDL_CreateWindow(title.data(), 0, 0, w * wscale, h * hscale, SDL_WINDOW_SHOWN);
		if (!window)
		{
			SDL_Quit();
			throw runtime_error{ SDL_GetError() };
		}
		window_ = sdl_window_pointer{ window };
	}

	// initialize the render
	{
		auto render = SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED);
		if (!render)
		{
			SDL_Quit();
			throw runtime_error{ SDL_GetError() };
		}

		renderer_ = sdl_renderer_pointer{ render };
	}

	// initialize the texture
	{
		auto texture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
		if (!texture)
		{
			SDL_Quit();
			throw runtime_error{ SDL_GetError() };
		}

		texture_ = sdl_texture_pointer{ texture };
	}

}

cchip8::display::display_window::~display_window()
{
	SDL_Quit();
}

