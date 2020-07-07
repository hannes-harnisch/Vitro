#pragma once

#include "Vitro/Utility/Ref.h"

namespace Vitro
{
	class UniformBuffer : public RefCounted
	{
	public:
		virtual ~UniformBuffer() = default;

		static Ref<UniformBuffer> New(const void* uniforms, size_t size);

		virtual void Bind() const						= 0;
		virtual void Update(const void* uniforms) const = 0;
	};
}
