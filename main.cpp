#include <iostream>
#include <chrono>

#include <argparse/argparse.hpp>

#include "display/display_window.h"
#include "processor/processor.h"

using namespace std;

using namespace cchip8::display;
using namespace cchip8::processor;

void run_emulator(argparse::ArgumentParser& arg_parser)
{
	const auto rom = arg_parser.get<string>("--rom");
	const auto vs = arg_parser.get<int>("--video-scale"), cd = arg_parser.get<int>("--cycle-delay");

	display_window win{ static_cast<uint32_t>(vs), "CHIP8" };

	machine emulator{};

	emulator.load(rom);

	auto last_cycle_time = chrono::high_resolution_clock::now();
	bool quit = false;
	while (!quit)
	{
		quit = win.process_events(emulator.keypad());

		auto time = chrono::high_resolution_clock::now();
		auto dt = std::chrono::duration<float, std::chrono::milliseconds::period>(time - last_cycle_time).count();

		if (dt > cd)
		{
			last_cycle_time = time;

			emulator.cycle();

			win.update(emulator.screen());
		}
	}
}

int main(int argc, char** argv)
{
	argparse::ArgumentParser arg_parser{ "cchip8" };

	arg_parser.add_argument("-r", "--rom")
			.help("the CHIP8 ROM file")
			.default_value(string{ "" })
			.implicit_value(string{ "" })
			.nargs(1);

	arg_parser.add_argument("-vs", "--video-scale")
			.help("Video scale.")
			.default_value(10)
			.implicit_value(10);

	arg_parser.add_argument("-cd", "--cycle-delay")
			.help("Delay per cycle.")
			.default_value(1)
			.implicit_value(1);

	try
	{
		arg_parser.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err)
	{
		cout << err.what() << endl;
		cout << arg_parser;
		return 1;
	}

	run_emulator(arg_parser);

	return 0;
}
