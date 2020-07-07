#include "_pch.h"

#if VTR_API_DIRECTX
	#include "Vitro/API/DirectX/Context3D.h"
	#include "Vitro/API/DirectX/FragmentShader.h"
	#include "Vitro/API/DirectX/IndexBuffer.h"
	#include "Vitro/API/DirectX/Texture2D.h"
	#include "Vitro/API/DirectX/UniformBuffer.h"
	#include "Vitro/API/DirectX/VertexBuffer.h"
	#include "Vitro/API/DirectX/VertexShader.h"
#endif

namespace Vitro
{
	Scope<Context3D> Context3D::New(void* nativeWindowHandle, uint32_t width, uint32_t height)
	{
#if VTR_API_DIRECTX
		return Scope<DirectX::Context3D>::New(nativeWindowHandle, width, height);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<FragmentShader> FragmentShader::New(const File& file)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::FragmentShader>::New(file);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<FragmentShader> FragmentShader::New(const std::string& sourceCode, std::string& errors)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::FragmentShader>::New(sourceCode, errors);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<IndexBuffer> IndexBuffer::New(const HeapArray<uint32_t>& indices)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::IndexBuffer>::New(indices);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<IndexBuffer> IndexBuffer::New(const uint32_t indices[], size_t count)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::IndexBuffer>::New(indices, count);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<Texture2D> Texture2D::New(const std::string& filePath)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::Texture2D>::New(filePath);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<UniformBuffer> UniformBuffer::New(const void* uniforms, size_t size)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::UniformBuffer>::New(uniforms, size);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<VertexBuffer> VertexBuffer::New(const void* vertices, size_t stride, size_t size)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::VertexBuffer>::New(vertices, stride, size);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<VertexShader> VertexShader::New(const File& file)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::VertexShader>::New(file);
#else
	#error Unsupported graphics API.
#endif
	}

	Ref<VertexShader> VertexShader::New(const std::string& sourceCode, std::string& errors)
	{
#if VTR_API_DIRECTX
		return Ref<DirectX::VertexShader>::New(sourceCode, errors);
#else
	#error Unsupported graphics API.
#endif
	}
}
