//
// Created by cleve on 1/13/2022.
//

#pragma once


#include <memory>
#include <coroutine>

#include <GLFW/glfw3.h>

namespace cchip8::display
{
/// \brief Owner class for display window
class display_window
{

public:

	display_window(int32_t w, int32_t h, std::string_view title);

	display_window(display_window&& another) noexcept:
			window_(std::exchange(another.window_, nullptr))
	{
	}

	~display_window();

	display_window(const display_window&) = delete;

	display_window& operator=(const display_window&) = delete;

	explicit operator bool() const
	{
		return window_;
	}

	bool operator!() const
	{
		return !window_;
	}

	[[nodiscard]] GLFWwindow* get_window() const
	{
		return window_;
	}


private:
	GLFWwindow* window_{ nullptr };
};
}