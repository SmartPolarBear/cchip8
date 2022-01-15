//
// Created by cleve on 1/13/2022.
//

#pragma once

#include <span>
#include <memory>
#include <coroutine>

#include <SDL.h>

#include "display/screen.h"

namespace cchip8::display
{
/// \brief Owner class for display window
class display_window
{
public:
	using sdl_window_pointer = std::unique_ptr<SDL_Window, decltype([](SDL_Window* w)
	{
		SDL_DestroyWindow(w);
	})>;

	using sdl_texture_pointer = std::unique_ptr<SDL_Texture, decltype([](SDL_Texture* t)
	{
		SDL_DestroyTexture(t);
	})>;

	using sdl_renderer_pointer = std::unique_ptr<SDL_Renderer, decltype([](SDL_Renderer* r)
	{
		SDL_DestroyRenderer(r);
	})>;
public:
	display_window(uint32_t scale, std::string_view title);

	display_window(display_window&& another) noexcept:
			window_(std::exchange(another.window_, nullptr))
	{
	}

	~display_window();

	display_window(const display_window&) = delete;

	display_window& operator=(const display_window&) = delete;

	explicit operator bool() const
	{
		return !(!window_);
	}

	bool operator!() const
	{
		return !window_;
	}

	[[nodiscard]] SDL_Window* get_window() const
	{
		return window_.get();
	}

	void update(const screen &scr);

	[[nodiscard]] bool process_events(std::span<uint8_t> keys);


private:
	sdl_window_pointer window_{ nullptr };
	sdl_texture_pointer texture_{ nullptr };
	sdl_renderer_pointer renderer_{ nullptr };
};
}