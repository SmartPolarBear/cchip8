//
// Created by cleve on 1/13/2022.
//

#pragma once

#include <span>
#include <cstdint>

namespace cchip8::display
{
class screen
{
public:
	static size_t constexpr WIDTH = 64;
	static size_t constexpr HEIGHT = 32;

	explicit screen(uint32_t *vmem);

	~screen()=default;

	bool toggle_pixel(int x,int y);

	uint32_t get_pixel(int x,int y);

private:
	// 64x32 monochrome pixels are represented
	std::span<uint32_t> pixels_{};
};
}