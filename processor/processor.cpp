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
		: reg_pc_(START_ADDRESS),
		  rand_gen_(system_clock::now().time_since_epoch().count()),
		  rand_byte_(0, 255U)
{
	memcpy(memory_ + FONTSET_ADDRESS, FONTSET, sizeof(FONTSET));
}

void machine::fetch_and_add()
{
	opcode_ = (memory_[reg_pc_] << 8u) | memory_[reg_pc_ + 1];
	reg_pc_ += INSTRUCTION_SIZE;
}

void machine::cycle()
{
	fetch_and_add();

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
}


void machine::dispatch_opcode_0()
{
	const auto opcode = current_opcode();
	(this->*OP_0[opcode & 0x000Fu])();
}

void machine::dispatch_opcode_8()
{
	const auto opcode = current_opcode();
	(this->*OP_8[opcode & 0x000Fu])();
}

void machine::dispatch_opcode_e()
{
	const auto opcode = current_opcode();
	(this->*OP_E[opcode & 0x000Fu])();
}

void machine::dispatch_opcode_f()
{
	const auto opcode = current_opcode();
	(this->*OP_F[opcode & 0x00FFu])();
}

void machine::dispatch_opcode()
{
	const auto opcode = current_opcode();
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

		memmove(memory_ + START_ADDRESS, buf.get(), size);
	}
}

void machine::op_default()
{
	// TODO: logging and/or sounding
	int fuck = 0;
}

// 00e0: clear the screen
void machine::op_00e0()
{
	memset(vmem_, 0, sizeof(vmem_));
}

// 00ee: return from subroutine
void machine::op_00ee()
{
	reg_pc_ = stack_[--reg_sp_];
}

// 1nnn jump to nnn
void machine::op_1nnn()
{
	const auto opcode = current_opcode();

	const auto addr = opcode & 0x0FFFu;
	reg_pc_ = addr;
}

//2nnn: call subroutine 2nnn
void machine::op_2nnn()
{
	const auto opcode = current_opcode();
	const auto addr = opcode & 0x0FFFu;

	stack_[reg_sp_++] = reg_pc_;
	reg_pc_ = addr;
}

// 3xkk: skip if reg[x]==kk
void machine::op_3xkk()
{
	const auto opcode = current_opcode();

	const auto reg_num = ((opcode & 0x0F00u) >> 8u);
	const auto val = (opcode & 0x00FFu);

	if (registers_[reg_num] == val)
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// 4xkk: skip if reg[x]!=kk
void machine::op_4xkk()
{
	const auto opcode = current_opcode();

	const auto reg_num = ((opcode & 0x0F00u) >> 8u);
	const auto val = (opcode & 0x00FFu);

	if (registers_[reg_num] != val)
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// 5xy0: skip if reg[x]==reg[y]
void machine::op_5xy0()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	if (registers_[reg1] == registers_[reg2])
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// set reg[x]=kk
void machine::op_6xkk()
{
	const auto opcode = current_opcode();

	const auto reg_num = ((opcode & 0x0F00u) >> 8u);
	const auto val = (opcode & 0x00FFu);

	registers_[reg_num] = val;
}

// set reg[x]+=kk
void machine::op_7xkk()
{
	const auto opcode = current_opcode();

	const auto reg_num = ((opcode & 0x0F00u) >> 8u);
	const auto val = (opcode & 0x00FFu);

	registers_[reg_num] += val;
}

// set reg[x]=reg[y]
void machine::op_8xy0()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	registers_[reg1] = registers_[reg2];
}

// set reg[x]|=reg[y]
void machine::op_8xy1()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	registers_[reg1] |= registers_[reg2];
}

// set reg[x]&=reg[y]
void machine::op_8xy2()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	registers_[reg1] &= registers_[reg2];
}

// set reg[x]^=reg[y]
void machine::op_8xy3()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	registers_[reg1] ^= registers_[reg2];
}

// set reg[x]+=reg[y]. affect carry flag
void machine::op_8xy4()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	uint32_t result = registers_[reg1] + registers_[reg2];

	registers_[VF] = result > UINT8_MAX;
	registers_[reg1] = result & 0xFF; // low 8 bits is set
}

// set reg[x]=reg[x]-reg[y]. affect carry flag
void machine::op_8xy5()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);


	registers_[VF] = registers_[reg1] > registers_[reg2];
	registers_[reg1] -= registers_[reg2];
}

// 8xy6: ignore y, rshift reg[x]  1 bit, cflag=the discarded bit
void machine::op_8xy6()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	[[maybe_unused]] const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	registers_[VF] = (registers_[reg1] & 0x1u);
	registers_[reg1] >>= 1;
}

// set reg[x]=reg[y]-reg[x]. affect carry flag
void machine::op_8xy7()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	registers_[VF] = registers_[reg2] > registers_[reg1];
	registers_[reg1] = registers_[reg2] - registers_[reg1];
}

// 8xye: ignore y, lshift reg[x]  1 bit, cflag=the discarded bit
void machine::op_8xye()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	[[maybe_unused]] const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	registers_[VF] = (registers_[reg1] & 0b1000'0000u);
	registers_[reg1] <<= 1;
}

void machine::op_9xy0()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	if (registers_[reg1] != registers_[reg2])
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// annn set index to nnn
void machine::op_annn()
{
	const auto opcode = current_opcode();

	const auto val = (opcode & 0x0FFFu);
	reg_i_ = val;
}

//bnnn: jump with offset. pc=reg[0]+nnn
void machine::op_bnnn()
{
	const auto opcode = current_opcode();
	auto address = opcode & 0x0FFFu;

	reg_pc_ = registers_[0] + address;
}

// cxkk: generate a number, bitwise and with nn and put into reg[x]
void machine::op_cxkk()
{
	const auto opcode = current_opcode();

	const auto reg_num = ((opcode & 0x0F00u) >> 8u);
	const auto val = (opcode & 0x00FFu);

	registers_[reg_num] = rand_byte_(rand_gen_) & val;
}

// dxyn: draw n pixels tall sprite from memory[reg_i] at (reg[x],reg[y]). VF=1 if some pixels is turned off
void machine::op_dxyn()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);
	const auto reg2 = ((opcode & 0x00F0u) >> 4u);

	const auto height = (opcode & 0x000Fu);

	const auto x = registers_[reg1] & 0x3Fu, // less than 63,
	y = registers_[reg2] & 0x1Fu; // less than 31

	registers_[VF] = 0;

	for (auto r = 0; r < height; r++)
	{
		auto byte = memory_[reg_i_ + r];
		for (auto c = 0; c < 8; c++)
		{
			auto pixel = byte & (0b1000'0000u >> c);
			if (pixel)
			{
				registers_[VF] = screen_.toggle_pixel(c + x, r + y);
			}
		}
	}
}

// exa1 skip if the key in reg[x] is not pressed
void machine::op_exa1()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	if (!keypad_[registers_[reg1]])
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// ex9e skip if the key in reg[x] is pressed
void machine::op_ex9e()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	if (keypad_[registers_[reg1]])
	{
		reg_pc_ += INSTRUCTION_SIZE;
	}
}

// fx07 set reg[x]=delay timer
void machine::op_fx07()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	registers_[reg1] = delay_timer_;
}

// fx0a: blocking getting
void machine::op_fx0a()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	bool set = false;
	for (int i = 0; i < KEY_COUNT; i++)
	{
		if (keypad_[i])
		{
			registers_[reg1] = i;
			set = true;
		}
	}

	if (!set)
	{
		reg_pc_ -= INSTRUCTION_SIZE;// loop
	}
}

// fx15: set delay timer=reg[x]
void machine::op_fx15()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	delay_timer_ = registers_[reg1];
}

// fx18: set sound timer=reg[x]
void machine::op_fx18()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	sound_timer_ = registers_[reg1];
}

// fx1e: add reg[x] to index.
void machine::op_fx1e()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	reg_i_ += registers_[reg1];
}

// fx29:set index to the char in reg[x]
void machine::op_fx29()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	reg_i_ = FONTSET_ADDRESS + (5 * registers_[reg1]);
}

// fx33: convert reg[x] to three decimal digits in memory[reg_i]
void machine::op_fx33()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	auto value = registers_[reg1];

	// Ones-place
	memory_[reg_i_ + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory_[reg_i_ + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory_[reg_i_] = value % 10;
}

// fx55 load v0 to vx in memory[reg_i]
void machine::op_fx55()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	for (int i = 0; i < reg1; i++)
	{
		memory_[reg_i_ + i] = registers_[i];
	}
}

// fx55 load memory[reg_i] to v0 to vx
void machine::op_fx65()
{
	const auto opcode = current_opcode();

	const auto reg1 = ((opcode & 0x0F00u) >> 8u);

	for (int i = 0; i < reg1; i++)
	{
		registers_[i] = memory_[reg_i_ + i];
	}
}

std::span<uint8_t> machine::keypad() const
{
	return { keypad_ };
}

cchip8::display::screen& machine::screen() const
{
	return screen_;
}



