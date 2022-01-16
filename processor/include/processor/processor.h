//
// Created by cleve on 1/14/2022.
//

#pragma once

#include "display/screen.h"

#include <span>
#include <random>
#include <cstdint>

namespace cchip8::processor
{
class machine
{
public:
	enum predefined_addresses : uint16_t
	{
		FONTSET_ADDRESS = 0x50,
		START_ADDRESS = 0x200,
	};

	enum audio_frequencies
	{
		TIMER_BEEP = 440,
		ERROR_BEEP = 350,
	};

	static constexpr size_t BEEP_DURATION = 20;

	enum general_purpose_registers
	{
		V0,
		V1,
		V2,
		V3,
		V4,
		V5,
		V6,
		V7,
		V8,
		V9,
		VA,
		VB,
		VC,
		VD,
		VE,
		VF,
	};

	static constexpr size_t MEMORY_IN_BYTES = 4096;

	static constexpr size_t STACK_SIZE = 1024;

	static constexpr size_t REGS_COUNT = 16;

	static constexpr size_t KEY_COUNT = 16;

	static constexpr size_t VMEM_SIZE = display::screen::WIDTH * display::screen::HEIGHT;

	static constexpr size_t INSTRUCTION_SIZE = sizeof(uint8_t[2]);

	static constexpr size_t FONTSET_SIZE = 80;

	using opcode_handle_type = void (machine::*)();

	machine();

	void cycle();

	void load(std::string_view filename);

	std::span<uint8_t> keypad() const;

	display::screen& screen() const;

private:
	[[nodiscard]] inline constexpr uint16_t current_opcode() const
	{
		return opcode_;
	}

	void fetch_and_add();

	void op_00e0();

	void op_00ee();

	void op_1nnn();

	void op_2nnn();

	void op_3xkk();

	void op_4xkk();

	void op_5xy0();

	void op_6xkk();

	void op_7xkk();

	void op_8xy0();

	void op_8xy1();

	void op_8xy2();

	void op_8xy3();

	void op_8xy4();

	void op_8xy5();

	void op_8xy6();

	void op_8xy7();

	void op_8xye();

	void op_9xy0();

	void op_annn();

	void op_bnnn();

	void op_cxkk();

	void op_dxyn();

	void op_exa1();

	void op_ex9e();

	void op_fx07();

	void op_fx0a();

	void op_fx15();

	void op_fx18();

	void op_fx1e();

	void op_fx29();

	void op_fx33();

	void op_fx55();

	void op_fx65();

	void op_default();

	void dispatch_opcode_0();

	void dispatch_opcode_8();

	void dispatch_opcode_e();

	void dispatch_opcode_f();

	void dispatch_opcode();


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-designator"
#pragma clang diagnostic ignored "-Winitializer-overrides"

	// predefined data
	static inline constexpr uint8_t FONTSET[FONTSET_SIZE] =
			{
					0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
					0x20, 0x60, 0x20, 0x20, 0x70, // 1
					0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
					0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
					0x90, 0x90, 0xF0, 0x10, 0x10, // 4
					0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
					0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
					0xF0, 0x10, 0x20, 0x40, 0x40, // 7
					0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
					0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
					0xF0, 0x90, 0xF0, 0x90, 0x90, // A
					0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
					0xF0, 0x80, 0x80, 0x80, 0xF0, // C
					0xE0, 0x90, 0x90, 0x90, 0xE0, // D
					0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
					0xF0, 0x80, 0xF0, 0x80, 0x80  // F
			};

	static inline constexpr opcode_handle_type OP_MAIN[0xF + 1]
			{
					[0 ... 0xF]=&machine::op_default,

					[0x0]=&machine::dispatch_opcode_0,
					[0x1]=&machine::op_1nnn,
					[0x2]=&machine::op_2nnn,
					[0x3]=&machine::op_3xkk,
					[0x4]=&machine::op_4xkk,
					[0x5]=&machine::op_5xy0,
					[0x6]=&machine::op_6xkk,
					[0x7]=&machine::op_7xkk,
					[0x8]=&machine::dispatch_opcode_8,
					[0x9]=&machine::op_9xy0,
					[0xA]=&machine::op_annn,
					[0xB]=&machine::op_bnnn,
					[0xC]=&machine::op_cxkk,
					[0xD]=&machine::op_dxyn,
					[0xE]=&machine::dispatch_opcode_e,
					[0xF]=&machine::dispatch_opcode_f,
			};


	static inline constexpr opcode_handle_type OP_0[0xE+1]{
			[0 ... 0xE]=&machine::op_default,

			[0x0]=&machine::op_00e0,
			[0xE]=&machine::op_00ee
	};

	static inline constexpr opcode_handle_type OP_8[0xE+1]{
			[0 ... 0xE]=&machine::op_default,

			[0x0]=&machine::op_8xy0,
			[0x1]=&machine::op_8xy1,
			[0x2]=&machine::op_8xy2,
			[0x3]=&machine::op_8xy3,
			[0x4]=&machine::op_8xy4,
			[0x5]=&machine::op_8xy5,
			[0x6]=&machine::op_8xy6,
			[0x7]=&machine::op_8xy7,
			[0xE]=&machine::op_8xye,
	};

	static inline constexpr opcode_handle_type OP_E[0xE+1]{
			[0 ... 0xE]=&machine::op_default,

			[0x1]=&machine::op_exa1,
			[0xE]=&machine::op_ex9e,
	};

	static inline constexpr opcode_handle_type OP_F[0x65 + 1]{
			[0 ... 0x65]=&machine::op_default,

			[0x07] = &machine::op_fx07,
			[0x0A] = &machine::op_fx0a,
			[0x15] = &machine::op_fx15,
			[0x18] = &machine::op_fx18,
			[0x1E] = &machine::op_fx1e,
			[0x29] = &machine::op_fx29,
			[0x33] = &machine::op_fx33,
			[0x55] = &machine::op_fx55,
			[0x65] = &machine::op_fx65,
	};
#pragma clang diagnostic pop
	// according to CHIP8 specification
	uint8_t registers_[REGS_COUNT]{};

	uint8_t memory_[MEMORY_IN_BYTES]{};
	uint32_t vmem_[VMEM_SIZE]{};
	uint16_t stack_[STACK_SIZE]{};

	mutable uint8_t keypad_[KEY_COUNT]{};

	uint16_t reg_pc_{ START_ADDRESS };
	uint16_t reg_i_{};
	uint16_t reg_sp_{};

	uint8_t delay_timer_{};
	uint8_t sound_timer_{};

	// for convenience

	uint16_t opcode_{};

	mutable display::screen screen_{ vmem_ };

	std::default_random_engine rand_gen_;
	std::uniform_int_distribution<uint32_t> rand_byte_{ 0, UINT8_MAX };

};
}