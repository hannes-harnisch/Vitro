#pragma once

#include "Vitro/API/DirectX/RHI.h"
#include "Vitro/API/DirectX/Scope.h"
#include "Vitro/Graphics/VertexBuffer.h"
#include "Vitro/Utility/HeapArray.h"

#include <d3d11_4.h>

namespace Vitro::DirectX
{
	class VertexBuffer final : public Vitro::VertexBuffer
	{
	public:
		VertexBuffer(const void* vertices, size_t stride, size_t size);

		void Bind(VertexTopology vt) const override;
		void Update(const void* vertices) const override;

	private:
		Scope<ID3D11Buffer> Buffer;
		uint32_t Stride;
		uint32_t Size;
	};
}
