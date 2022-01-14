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

	[[nodiscard]] inline uint16_t opcode() const
	{
		return opcode_view_[reg_pc_];
	}

private:
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