//
// Created by cleve on 1/16/2022.
//

#pragma once

#include <queue>
#include <utility>
#include <span>

#include <SDL.h>
#include <SDL_audio.h>

namespace cchip8::sound
{
class beeper
{
public:
	static constexpr int AMPLITUDE = 28000;
	static constexpr int FREQUENCY = 44100;

	beeper();

	~beeper();

	beeper(beeper&&) = delete;

	beeper(const beeper&) = delete;

	beeper& operator=(const beeper&) = delete;

	void beep(double freq,int duration);

	void wait();

private:
	void sample(std::span<Sint16> stream);
	static void callback(void*,Uint8*,int);

	double v_{};
	std::queue<std::pair<double, int>> beep_segs_{};
};
}