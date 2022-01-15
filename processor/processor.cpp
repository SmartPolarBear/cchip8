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


void machine::dispatch_opcode_0()
{
	auto opcode = current_opcode();
	(this->*OP_0[opcode & 0x000Fu])();
}

void machine::dispatch_opcode_8()
{
	auto opcode = current_opcode();
	(this->*OP_8[opcode & 0x000Fu])();
}

void machine::dispatch_opcode_e()
{
	auto opcode = current_opcode();
	(this->*OP_E[opcode & 0x000Fu])();
}

void machine::dispatch_opcode_f()
{
	auto opcode = current_opcode();
	(this->*OP_F[opcode & 0x00FFu])();
}

void machine::dispatch_opcode()
{
	auto opcode = current_opcode();
	(this->*OP_MAIN[(opcode & 0xF000u) >> 12u])();
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

void machine::op_default()
{
	// TODO: logging and/or sounding
}

void machine::op_00e0()
{

}

void machine::op_00ee()
{

}

void machine::op_1nnn()
{

}

void machine::op_2nnn()
{

}

void machine::op_3xkk()
{

}

void machine::op_4xkk()
{

}

void machine::op_5xy0()
{

}

void machine::op_6xkk()
{

}

void machine::op_7xkk()
{

}

void machine::op_8xy0()
{

}

void machine::op_8xy1()
{

}

void machine::op_8xy2()
{

}

void machine::op_8xy3()
{

}

void machine::op_8xy4()
{

}

void machine::op_8xy5()
{

}

void machine::op_8xy6()
{

}

void machine::op_8xy7()
{

}

void machine::op_8xye()
{

}

void machine::op_9xy0()
{

}

void machine::op_annn()
{

}

void machine::op_bnnn()
{

}

void machine::op_cxkk()
{

}

void machine::op_dxyn()
{

}

void machine::op_exa1()
{

}

void machine::op_ex9e()
{

}

void machine::op_fx07()
{

}

void machine::op_fx0a()
{

}

void machine::op_fx15()
{

}

void machine::op_fx18()
{

}

void machine::op_fx1e()
{

}

void machine::op_fx29()
{

}

void machine::op_fx33()
{

}

void machine::op_fx55()
{

}

void machine::op_fx65()
{

}


