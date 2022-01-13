//
// Created by cleve on 1/13/2022.
//

#include <stdexcept>

#include "display/display_window.h"

using namespace std;

using namespace cchip8::display;

cchip8::display::display_window::display_window(int32_t w, int32_t h, std::string_view title)
{
	if (!glfwInit())
	{
		throw runtime_error{ "Cannot initialize GLFW" };
	}

	window_ = glfwCreateWindow(w, h, title.data(), nullptr, nullptr);
	if (!window_)
	{
		glfwTerminate();
		throw runtime_error{ "Cannot create GLFW window" };
	}

	glfwMakeContextCurrent(window_);
}

cchip8::display::display_window::~display_window()
{
	glfwTerminate();
}
