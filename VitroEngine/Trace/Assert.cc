#include "Assert.hh"

void crash(std::string_view const message)
{
	std::printf("Critical failure: %s", message.data());

	std::exit(EXIT_FAILURE);
}
