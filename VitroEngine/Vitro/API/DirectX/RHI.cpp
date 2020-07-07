#include "_pch.h"
#include "RHI.h"

#include "Vitro/Utility/Assert.h"

namespace Vitro::DirectX
{
	void RHI::Initialize()
	{
		static bool initialized = false;
		AssertCritical(!initialized, "DirectX API already initialized.");

		UINT flags = 0;
#if VTR_DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL levels[] {
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,	D3D_FEATURE_LEVEL_9_2,	D3D_FEATURE_LEVEL_9_1,
		};
		UINT levelCount = static_cast<UINT>(ArrayCount(levels));

		Scope<ID3D11Device> device;
		Scope<ID3D11DeviceContext> context;
		auto res = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, levels, levelCount,
									 D3D11_SDK_VERSION, &device, nullptr, &context);
		AssertCritical(SUCCEEDED(res), "DirectX device could not be created.");
		device->QueryInterface(IID_PPV_ARGS(&Device));
		context->QueryInterface(IID_PPV_ARGS(&Context));
#if VTR_DEBUG
		auto query = Device->QueryInterface(IID_PPV_ARGS(&DebugLayer));
		AssertCritical(SUCCEEDED(query), "Could not get debug layer.");
#endif

		D3D11_BLEND_DESC bd;
		bd.AlphaToCoverageEnable				 = false;
		bd.IndependentBlendEnable				 = false;
		bd.RenderTarget[0].BlendEnable			 = true;
		bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		Scope<ID3D11BlendState> blendState;
		Device->CreateBlendState(&bd, &blendState);

		float blendFactor[4] {0.0f, 0.0f, 0.0f, 0.0f};
		Context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

		initialized = true;
	}
}
