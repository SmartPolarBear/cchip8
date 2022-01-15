#include <iostream>

#include <argparse/argparse.hpp>

#include "display/display_window.h"

using namespace std;

using namespace cchip8::display;

int main(int argc,char **argv)
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
			.default_value(3)
			.implicit_value(3);

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

	return 0;
}
