//
// Created by cleve on 1/13/2022.
//

#include "display/screen.h"

cchip8::display::screen::screen(uint32_t* vmem)
		: pixels_(vmem, WIDTH * HEIGHT)
{
}

bool cchip8::display::screen::toggle_pixel(int x, int y)
{
	auto ret = (pixels_[y * display::screen::WIDTH + x] == 0xFFFF'FFFF);
	pixels_[y * display::screen::WIDTH + x] ^= 0xFFFF'FFFF;
	return ret;
}

uint32_t cchip8::display::screen::get_pixel(int x, int y)
{
	return 0;
}

std::span<uint32_t> cchip8::display::screen::pixels() const
{
	return pixels_;
}
