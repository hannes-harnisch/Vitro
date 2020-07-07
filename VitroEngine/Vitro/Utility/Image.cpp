#include "_pch.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_HDR
#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#include <stb/stb_image.h>

namespace Vitro
{
	Image::Image(const std::string& filePath, uint32_t& width, uint32_t& height)
	{
		int w, h;
		Data   = stbi_load(filePath.c_str(), &w, &h, nullptr, 0);
		width  = w;
		height = h;
	}

	Image::Image(Image&& other) noexcept : Data(std::exchange(other.Data, nullptr))
	{}

	Image::~Image()
	{
		stbi_image_free(Data);
	}

	Image& Image::operator=(Image&& other) noexcept
	{
		std::swap(Data, other.Data);
		return *this;
	}
}
