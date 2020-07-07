#include "_pch.h"
#include "File.h"

namespace Vitro
{
	File::File(const std::string& path) : Path(path)
	{}

	std::string File::GetName() const
	{
		return std::filesystem::path(Path).filename().string();
	}

	std::string File::GetStem() const
	{
		return std::filesystem::path(Path).stem().string();
	}

	HeapArray<char> File::LoadBinary() const
	{
		FILE* file;
		fopen_s(&file, Path.c_str(), "rb");
		if(!file)
			return HeapArray<char>();

		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		rewind(file);

		HeapArray<char> data(size);
		fread(data.Raw(), size, 1, file);
		fclose(file);
		return data;
	}
}
