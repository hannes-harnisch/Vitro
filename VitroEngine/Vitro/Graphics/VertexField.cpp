#include "_pch.h"
#include "VertexField.h"

#include "Vitro/Math/Vector.h"

namespace Vitro
{
	VertexField::VertexField(const char name[], uint32_t index, VertexFieldType type, bool normalized) :
		Name(name), Index(index), Type(type), Normalized(normalized), StorageSize(GetSizeOf(type)), Offset(0)
	{}

	uint32_t VertexField::GetSizeOf(VertexFieldType type)
	{
		switch(type)
		{
			case VertexFieldType::Float: return sizeof(float);
			case VertexFieldType::Float2: return sizeof(Float2);
			case VertexFieldType::Float3: return sizeof(Float3);
			case VertexFieldType::Float4: return sizeof(Float4);
			case VertexFieldType::Int: return sizeof(int);
			case VertexFieldType::Int2: return sizeof(Int2);
			case VertexFieldType::Int3: return sizeof(Int3);
			case VertexFieldType::Int4: return sizeof(Int4);
			case VertexFieldType::Bool: return sizeof(bool);
			default: AssertCritical(false, "Unknown vertex field type.");
		}
		return 0;
	}
}
