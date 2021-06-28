module;
#include <string>
#include <string_view>
export module Vitro.Graphics.AdapterBase;

export class AdapterBase
{
public:
	virtual std::string_view getName() const = 0;
	virtual size_t getVRAM() const			 = 0;
};
