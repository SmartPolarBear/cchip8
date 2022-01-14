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

private:
	// 64x32 monochrome pixels are represented
	std::span<uint32_t> pixels_{};
};
}