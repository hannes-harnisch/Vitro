#pragma once

#include "Vitro/Utility/HeapArray.h"

namespace Vitro
{
	// Wrapper for C file IO functions.
	class File final
	{
	public:
		File(const std::string& path);

		std::string GetName() const;
		std::string GetStem() const;
		HeapArray<char> LoadBinary() const;

	private:
		std::string Path;
	};
}
