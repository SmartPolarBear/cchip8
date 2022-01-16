//
// Created by cleve on 1/13/2022.
//

#include <stdexcept>

#include "display/display_window.h"
#include "display/screen.h"

using namespace std;

using namespace cchip8::display;

cchip8::display::display_window::display_window(uint32_t scale, std::string_view title)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		auto e = SDL_GetError();
		throw runtime_error{ SDL_GetError() };
	}

	// initialize the window
	{
		auto window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				screen::WIDTH * scale, screen::HEIGHT * scale,
				SDL_WINDOW_SHOWN);
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
		auto texture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				screen::WIDTH, screen::HEIGHT);
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
	texture_.release();
	renderer_.release();
	window_.release();
	
	SDL_Quit();
}

void display_window::update(const screen& scr)
{
	if (SDL_UpdateTexture(texture_.get(), nullptr, scr.pixels().data(), screen::PITCH) < 0)
	{
		throw runtime_error{ SDL_GetError() };
	}

	if (SDL_RenderClear(renderer_.get()) < 0)
	{
		throw runtime_error{ SDL_GetError() };
	}

	if (SDL_RenderCopy(renderer_.get(), texture_.get(), nullptr, nullptr) < 0)
	{
		throw runtime_error{ SDL_GetError() };
	}

	SDL_RenderPresent(renderer_.get());
}

bool display_window::process_events(std::span<uint8_t> keys)
{
	bool quit = false;

	SDL_Event e{};
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		default:
			break;

		case SDL_QUIT:
		{
			quit = true;
			break;
		}

		case SDL_KEYDOWN:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			{
				quit = true;
				break;
			}

			case SDLK_x:
			{
				keys[0] = 1;
				break;
			}

			case SDLK_1:
			{
				keys[1] = 1;
				break;
			}

			case SDLK_2:
			{
				keys[2] = 1;
				break;
			}

			case SDLK_3:
			{
				keys[3] = 1;
				break;
			}

			case SDLK_q:
			{
				keys[4] = 1;
				break;
			}

			case SDLK_w:
			{
				keys[5] = 1;
				break;
			}

			case SDLK_e:
			{
				keys[6] = 1;
				break;
			}

			case SDLK_a:
			{
				keys[7] = 1;
				break;
			}

			case SDLK_s:
			{
				keys[8] = 1;
				break;
			}

			case SDLK_d:
			{
				keys[9] = 1;
				break;
			}

			case SDLK_z:
			{
				keys[0xA] = 1;
				break;
			}

			case SDLK_c:
			{
				keys[0xB] = 1;
				break;
			}

			case SDLK_4:
			{
				keys[0xC] = 1;
				break;
			}

			case SDLK_r:
			{
				keys[0xD] = 1;
				break;
			}

			case SDLK_f:
			{
				keys[0xE] = 1;
				break;
			}

			case SDLK_v:
			{
				keys[0xF] = 1;
				break;
			}

			}
			break;
		}

		case SDL_KEYUP:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_x:
			{
				keys[0] = 0;
				break;
			}

			case SDLK_1:
			{
				keys[1] = 0;
				break;
			}

			case SDLK_2:
			{
				keys[2] = 0;
				break;
			}

			case SDLK_3:
			{
				keys[3] = 0;
				break;
			}

			case SDLK_q:
			{
				keys[4] = 0;
				break;
			}

			case SDLK_w:
			{
				keys[5] = 0;
				break;
			}

			case SDLK_e:
			{
				keys[6] = 0;
				break;
			}

			case SDLK_a:
			{
				keys[7] = 0;
				break;
			}

			case SDLK_s:
			{
				keys[8] = 0;
				break;
			}

			case SDLK_d:
			{
				keys[9] = 0;
				break;
			}

			case SDLK_z:
			{
				keys[0xA] = 0;
				break;
			}

			case SDLK_c:
			{
				keys[0xB] = 0;
				break;
			}

			case SDLK_4:
			{
				keys[0xC] = 0;
				break;
			}

			case SDLK_r:
			{
				keys[0xD] = 0;
				break;
			}

			case SDLK_f:
			{
				keys[0xE] = 0;
				break;
			}

			case SDLK_v:
			{
				keys[0xF] = 0;
				break;
			}
			}
			break;
		}
		}
	}

	return quit;
}

