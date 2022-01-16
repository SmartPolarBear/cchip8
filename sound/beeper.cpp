//
// Created by cleve on 1/16/2022.
//

#include "sound/beeper.h"

#include <cmath>
#include <stdexcept>

using namespace std;

cchip8::sound::beeper::beeper()
{
	SDL_AudioSpec spec;

	spec.freq = FREQUENCY;
	spec.format = AUDIO_S16SYS;
	spec.channels = 1;
	spec.samples = 2048;
	spec.callback = callback;
	spec.userdata = this;

	SDL_AudioSpec obtained;

	// you might want to look for errors here
	if (SDL_OpenAudio(&spec, &obtained) != 0)
	{
		throw runtime_error{ SDL_GetError() };
	}

	// start play audio
	SDL_PauseAudio(0);
}

cchip8::sound::beeper::~beeper()
{
	SDL_CloseAudio();
}

void cchip8::sound::beeper::beep(double freq, int duration)
{
	SDL_LockAudio();
	beep_segs_.emplace(freq, duration * FREQUENCY / 1000);
	SDL_UnlockAudio();
}

void cchip8::sound::beeper::wait()
{
	int size = 0;
	do
	{
		SDL_Delay(20);
		SDL_LockAudio();
		size = beep_segs_.size();
		SDL_UnlockAudio();
	} while (size > 0);
}

void cchip8::sound::beeper::sample(std::span<Sint16> stream)
{
	int i = 0;
	while (i < stream.size())
	{

		if (beep_segs_.empty())
		{
			while (i < stream.size())
			{
				stream[i] = 0;
				i++;
			}
			return;
		}
		auto& bo = beep_segs_.front();

		int samples_count = std::min(i + bo.second, (int)stream.size());
		bo.second -= samples_count - i;

		while (i < samples_count)
		{
			stream[i] = AMPLITUDE * std::sin(v_ * 2 * 3.1415926f / FREQUENCY);
			i++;
			v_ += bo.first;
		}

		if (bo.second == 0)
		{
			beep_segs_.pop();
		}
	}
}

void cchip8::sound::beeper::callback(void* b, Uint8* s, int len)
{
	auto* stream = (Sint16*)s;
	int length = len / 2;
	auto* bp = (beeper*)b;

	bp->sample({ stream, (size_t)length });
}
