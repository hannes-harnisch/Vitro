#include <compare>
#include <exception>
#include <iostream>
#include <string_view>
#include <vector>

import vttool.HlslBuilder.BuildDispatch;
import vttool.HlslBuilder.InputArgs;

int main(int argc, char* argv[])
{
	try
	{
		std::vector<std::string_view> args(argv + 1, argv + argc);

		vt::tool::InputArgs input(args);
		vt::tool::dispatch_build(input);
		std::cout << "Compilation successful." << std::endl;

		return EXIT_SUCCESS;
	}
	catch(std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
