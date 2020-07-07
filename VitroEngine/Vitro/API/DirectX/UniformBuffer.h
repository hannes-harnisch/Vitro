#pragma once

#include "Vitro/API/DirectX/RHI.h"
#include "Vitro/API/DirectX/Scope.h"
#include "Vitro/Graphics/UniformBuffer.h"
#include "Vitro/Utility/Assert.h"

namespace Vitro::DirectX
{
	class UniformBuffer final : public Vitro::UniformBuffer
	{
	public:
		UniformBuffer(const void* uniforms, size_t size);

		void Bind() const override;
		void Update(const void* uniforms) const override;

	private:
		Scope<ID3D11Buffer> Buffer;
	};
}
