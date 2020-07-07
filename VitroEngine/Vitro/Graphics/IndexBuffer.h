#pragma once

#include "Vitro/Utility/HeapArray.h"
#include "Vitro/Utility/Ref.h"

namespace Vitro
{
	class IndexBuffer : public RefCounted
	{
	public:
		virtual ~IndexBuffer() = default;

		static Ref<IndexBuffer> New(const HeapArray<uint32_t>& indices);
		static Ref<IndexBuffer> New(const uint32_t indices[], size_t count);

		virtual void Bind() const	   = 0;
		virtual uint32_t Count() const = 0;
	};
}
