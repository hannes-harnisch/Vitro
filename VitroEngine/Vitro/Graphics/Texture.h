#pragma once

#include "Vitro/Utility/Ref.h"

namespace Vitro
{
	class Texture : public RefCounted
	{
	public:
		virtual ~Texture()		  = default;
		virtual void Bind() const = 0;

		uint32_t GetWidth() const
		{
			return Width;
		}
		uint32_t GetHeight() const
		{
			return Height;
		}

		uint32_t GetMipCount() const
		{
			uint32_t levels = 1;
			while((Width | Height) >> levels)
				levels++;
			return levels;
		}

	protected:
		uint32_t Width	= 0;
		uint32_t Height = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> New(const std::string& filePath);
	};
}
