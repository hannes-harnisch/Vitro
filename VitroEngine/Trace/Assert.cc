#include "Assert.hh"

void crash(const std::string_view message)
{
	std::printf("Critical failure: %s", message.data());

	std::exit(EXIT_FAILURE);
}
