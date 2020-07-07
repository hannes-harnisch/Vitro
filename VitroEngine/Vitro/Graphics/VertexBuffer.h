#pragma once

#include "Vitro/Graphics/VertexTopology.h"
#include "Vitro/Utility/Ref.h"

namespace Vitro
{
	class VertexBuffer : public RefCounted
	{
	public:
		virtual ~VertexBuffer() = default;

		static Ref<VertexBuffer> New(const void* vertices, size_t stride, size_t size);

		virtual void Bind(VertexTopology vt) const		= 0;
		virtual void Update(const void* vertices) const = 0;
	};
}
