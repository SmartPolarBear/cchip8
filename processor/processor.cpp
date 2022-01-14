//
// Created by cleve on 1/14/2022.
//
#include <fstream>
#include <iostream>
#include <chrono>

#include <gsl/gsl>

#include "processor/processor.h"

using namespace std;
using namespace std::chrono;

using namespace gsl;

using namespace cchip8::processor;

machine::machine()
		: rand_gen_(system_clock::now().time_since_epoch().count())
{
}

void machine::cycle()
{
	dispatch_opcode();

	// decrement timers

	if (delay_timer_ > 0)
	{
		delay_timer_ -= 1;
	}

	if (sound_timer_ > 0)
	{
		// TODO: go off a beep
		sound_timer_ -= 1;
	}

	reg_pc_ += INSTRUCTION_SIZE;
}

void machine::dispatch_opcode()
{
	auto opcode = current_opcode();
}

void machine::load(std::string_view filename)
{
	ifstream rom{ filename.data(), ios::binary | ios::ate };

	auto _ = finally([&rom]
	{
		rom.close();
	});

	if (rom.is_open())
	{
		auto size = rom.tellg();

		auto buf = make_unique<char[]>(size);

		rom.seekg(0, ios::beg);
		rom.read(buf.get(), size);

		memmove(memory_, buf.get(), size);
	}
}

