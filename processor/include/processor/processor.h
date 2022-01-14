//
// Created by cleve on 1/14/2022.
//

#pragma once

#include <span>
#include <cstdint>

namespace cchip8::processor
{
class machine
{
public:
	enum predefined_addresses : uint32_t
	{
		FONTSET_ADDRESS = 0x50,
		START_ADDRESS = 0x200,
	};

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

	static constexpr size_t INSTRUCTION_SIZE = sizeof(uint8_t[2]);

	static constexpr size_t FONTSET_SIZE = 80;

	[[nodiscard]] inline uint16_t opcode() const
	{
		return opcode_view_[reg_pc_];
	}

	void cycle()
	{

		// decrement timers

		if (delay_timer_ > 0)
		{
			delay_timer_ -= 1;
		}

		if (sound_timer_ > 0)
		{
			sound_timer_ -= 1;
		}

		reg_pc_ += INSTRUCTION_SIZE;
	}

private:
	// predefined data
	static inline constexpr uint8_t fontset[FONTSET_SIZE] =
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

	// according to CHIP8 specification
	uint8_t registers_[REGS_COUNT]{};

	uint8_t memory_[MEMORY_IN_BYTES]{};
	uint8_t stack_[STACK_SIZE]{};

	uint16_t reg_pc_{};
	uint16_t reg_i_{};

	uint8_t delay_timer_{};
	uint8_t sound_timer_{};

	// for convenience

	std::span<uint16_t> opcode_view_{ reinterpret_cast<uint16_t*>(memory_), std::dynamic_extent };
};
}