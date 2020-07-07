#include "_pch.h"
#include "VertexBuffer.h"

#include "Vitro/API/DirectX/RHI.h"

namespace Vitro::DirectX
{
	VertexBuffer::VertexBuffer(const void* vertices, size_t stride, size_t size) :
		Stride(static_cast<uint32_t>(stride)), Size(static_cast<uint32_t>(size))
	{
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth		   = Size;
		bd.Usage			   = D3D11_USAGE_DEFAULT;
		bd.BindFlags		   = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags	   = 0;
		bd.MiscFlags		   = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem			 = vertices;
		srd.SysMemPitch		 = 0;
		srd.SysMemSlicePitch = 0;

		auto res = RHI::Device->CreateBuffer(&bd, &srd, &Buffer);
		AssertCritical(SUCCEEDED(res), "Could not create vertex buffer.");
	}

	void VertexBuffer::Bind(VertexTopology vt) const
	{
		UINT offset = 0;
		RHI::Context->IASetVertexBuffers(0, 1, &Buffer, &Stride, &offset);
		RHI::Context->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(vt));
	}

	void VertexBuffer::Update(const void* vertices) const
	{
		RHI::Context->UpdateSubresource(Buffer, 0, nullptr, vertices, 0, 0);
	}
}
