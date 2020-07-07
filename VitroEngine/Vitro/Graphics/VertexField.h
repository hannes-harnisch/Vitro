#pragma once

#include "Vitro/Graphics/VertexFieldType.h"

namespace Vitro
{
	struct VertexField final
	{
	public:
		static constexpr auto Position = "POSITION";
		static constexpr auto Color	   = "COLOR";
		static constexpr auto TexCoord = "TEXCOORD";

		std::string Name;
		uint32_t Index;
		VertexFieldType Type;
		bool Normalized;
		uint32_t StorageSize;
		uint32_t Offset;

		VertexField(const char name[], uint32_t index, VertexFieldType type, bool normalized = false);

	private:
		static uint32_t GetSizeOf(VertexFieldType type);
	};
}
