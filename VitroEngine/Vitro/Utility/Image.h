#pragma once

namespace Vitro
{
	class Image final
	{
	public:
		uint8_t* Data = nullptr;

		Image(const std::string& filePath, uint32_t& width, uint32_t& height);
		Image(Image&& other) noexcept;
		~Image();
		Image& operator=(Image&& other) noexcept;

		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
	};
}
