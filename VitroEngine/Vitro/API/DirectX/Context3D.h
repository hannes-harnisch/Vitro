#pragma once

#include "Vitro/API/DirectX/Scope.h"
#include "Vitro/Graphics/Context3D.h"
#include "Vitro/Graphics/IndexBuffer.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>

namespace Vitro::DirectX
{
	class Context3D final : public Vitro::Context3D
	{
	public:
		Context3D(void* nativeWindowHandle, uint32_t width, uint32_t height);

		void SetViewport(uint32_t width, uint32_t height, int x, int y) override;
		void SetClearColor(const Float4& color) override;
		void SetVSync(bool enabled) override;
		void TargetBackBuffer() override;
		void DrawIndices(const Ref<IndexBuffer>& ib) override;
		void SwapBuffers() override;

	private:
		Scope<IDXGISwapChain1> SwapChain;
		Scope<ID3D11RenderTargetView> BackBuffer;
		Scope<ID3D11DepthStencilView> DepthStencilBuffer;
		uint32_t SwapInterval = 0;
	};
}
