module;
#include <string>
export module Vitro.Graphics.Adapter;

export struct Adapter
{
	void* handle;
	std::string name;
	size_t videoMemory;
};
