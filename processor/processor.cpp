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

// 00e0: clear the screen
void machine::op_00e0()
{
	memset(vmem_, 0, sizeof(vmem_));
}

// 00ee: return from subroutine
void machine::op_00ee()
{
	reg_sp_--;
	reg_pc_ = stack_[reg_sp_];
}

// 1nnn jump to nnn
void machine::op_1nnn()
{
	auto opcode = current_opcode();

	auto addr = opcode & 0x0FFFu;
	reg_pc_ = addr;
}

//2nnn: call subroutine 2nnn
void machine::op_2nnn()
{
	auto opcode = current_opcode();

	stack_[reg_sp_++] = reg_pc_;

	auto addr = opcode & 0x0FFFu;
	reg_pc_ = addr;
}

// 3xkk: skip if reg[x]==kk
void machine::op_3xkk()
{
	auto opcode = current_opcode();

	auto reg_num = ((opcode & 0x0F00) >> 8);
	auto val = (opcode & 0x00FF);

	if (registers_[reg_num] == val)
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// 4xkk: skip if reg[x]!=kk
void machine::op_4xkk()
{
	auto opcode = current_opcode();

	auto reg_num = ((opcode & 0x0F00) >> 8);
	auto val = (opcode & 0x00FF);

	if (registers_[reg_num] != val)
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// 5xy0: skip if reg[x]==reg[y]
void machine::op_5xy0()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	if (registers_[reg1] == registers_[reg2])
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// set reg[x]=kk
void machine::op_6xkk()
{
	auto opcode = current_opcode();

	auto reg_num = ((opcode & 0x0F00) >> 8);
	auto val = (opcode & 0x00FF);

	registers_[reg_num] = val;
}

// set reg[x]+=kk
void machine::op_7xkk()
{
	auto opcode = current_opcode();

	auto reg_num = ((opcode & 0x0F00) >> 8);
	auto val = (opcode & 0x00FF);

	registers_[reg_num] += val;
}

// set reg[x]=reg[y]
void machine::op_8xy0()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	registers_[reg1] = registers_[reg2];
}

// set reg[x]|=reg[y]
void machine::op_8xy1()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	registers_[reg1] |= registers_[reg2];
}

// set reg[x]&=reg[y]
void machine::op_8xy2()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	registers_[reg1] &= registers_[reg2];
}

// set reg[x]^=reg[y]
void machine::op_8xy3()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	registers_[reg1] ^= registers_[reg2];
}

// set reg[x]+=reg[y]. affect carry flag
void machine::op_8xy4()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	uint32_t result = registers_[reg1] + registers_[reg2];

	registers_[VF] = result > UINT8_MAX;
	registers_[reg1] = result & 0xFF; // low 8 bits is set
}

// set reg[x]=reg[x]-reg[y]. affect carry flag
void machine::op_8xy5()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);


	registers_[VF] = registers_[reg1] > registers_[reg2];
	registers_[reg1] -= registers_[reg2];
}

// 8xy6: ignore y, rshift reg[x]  1 bit, cflag=the discarded bit
void machine::op_8xy6()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	[[maybe_unused]] auto reg2 = ((opcode & 0x00F0) >> 4);

	registers_[VF] = (registers_[reg1] & 0x1u);
	registers_[reg1] >>= 1;
}

// set reg[x]=reg[y]-reg[x]. affect carry flag
void machine::op_8xy7()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	registers_[VF] = registers_[reg2] > registers_[reg1];
	registers_[reg1] = registers_[reg2] - registers_[reg1];
}

// 8xye: ignore y, lshift reg[x]  1 bit, cflag=the discarded bit
void machine::op_8xye()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	[[maybe_unused]] auto reg2 = ((opcode & 0x00F0) >> 4);

	registers_[VF] = (registers_[reg1] & 0b1000'0000u);
	registers_[reg1] <<= 1;
}

void machine::op_9xy0()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	if (registers_[reg1] != registers_[reg2])
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// annn set index to nnn
void machine::op_annn()
{
	auto opcode = current_opcode();

	auto val = (opcode & 0x0fFF);
	reg_i_ = val;
}

//bnnn: jump with offset. pc=reg[0]+nnn
void machine::op_bnnn()
{
	auto opcode = current_opcode();
	auto address = opcode & 0x0FFFu;

	reg_pc_ = registers_[0] + address;
}

// cxkk: generate a number, bitwise and with nn and put into reg[x]
void machine::op_cxkk()
{
	auto opcode = current_opcode();

	auto reg_num = ((opcode & 0x0F00) >> 8);
	auto val = (opcode & 0x00FF);

	registers_[reg_num] = rand_byte_(rand_gen_) & val;
}

// dxyn: draw n pixels tall sprite from memory[reg_i] at (reg[x],reg[y]). VF=1 if some pixels is turned off
void machine::op_dxyn()
{
	auto opcode = current_opcode();

	auto reg1 = ((opcode & 0x0F00) >> 8);
	auto reg2 = ((opcode & 0x00F0) >> 4);

	auto height = (opcode & 0x000F);

	auto x = registers_[reg1] & 0x3F, // less than 63,
	y = registers_[reg2] & 0x1F; // less than 31

	registers_[VF] = 0;

	for (auto r = 0; r < height; r++)
	{
		auto byte = memory_[reg_i_ + r];
		for (auto c = 0; c < 8; c++)
		{
			if (r + y >= display::screen::HEIGHT || (c + x) >= display::screen::WIDTH)continue;

			auto pixel = byte & (0b1000'0000 >> c);
			auto screen_pixel = &vmem_[(r + y) * display::screen::WIDTH + (c + x)];

			if (*screen_pixel == 0xFFFF'FFFF)
			{
				registers_[VF] = 1;
			}
			*screen_pixel ^= 0xFFFF'FFFF;
		}
	}
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


