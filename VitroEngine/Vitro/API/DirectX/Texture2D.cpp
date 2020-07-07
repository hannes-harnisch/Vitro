#include "_pch.h"
#include "Texture2D.h"

#include "Vitro/API/DirectX/RHI.h"
#include "Vitro/Utility/Assert.h"
#include "Vitro/Utility/Image.h"

namespace Vitro::DirectX
{
	Texture2D::Texture2D(const std::string& filePath)
	{
		Image img(filePath, Width, Height);

		D3D11_TEXTURE2D_DESC t2dd;
		t2dd.Width				= Width;
		t2dd.Height				= Height;
		t2dd.MipLevels			= GetMipCount();
		t2dd.ArraySize			= 1;
		t2dd.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		t2dd.SampleDesc.Count	= 1;
		t2dd.SampleDesc.Quality = 0;
		t2dd.Usage				= D3D11_USAGE_DEFAULT;
		t2dd.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		t2dd.CPUAccessFlags		= 0;
		t2dd.MiscFlags			= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		auto texRes				= RHI::Device->CreateTexture2D(&t2dd, nullptr, &Texture);
		AssertDebug(SUCCEEDED(texRes), "Could not create 2D texture.");

		RHI::Context->UpdateSubresource(Texture, 0, nullptr, img.Data, Width * 4, 0);
		auto srvRes = RHI::Device->CreateShaderResourceView(Texture, nullptr, &Resource);
		AssertDebug(SUCCEEDED(srvRes), "Could not create shader resource view from texture.");
		RHI::Context->GenerateMips(Resource);

		D3D11_SAMPLER_DESC sd;
		sd.Filter		  = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU		  = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV		  = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW		  = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias	  = 1;
		sd.MaxAnisotropy  = 0;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.BorderColor[0] = 0;
		sd.BorderColor[1] = 0;
		sd.BorderColor[2] = 0;
		sd.BorderColor[3] = 0;
		sd.MinLOD		  = 0;
		sd.MaxLOD		  = D3D11_FLOAT32_MAX;
		auto samplerRes	  = RHI::Device->CreateSamplerState(&sd, &Sampler);
		AssertDebug(SUCCEEDED(samplerRes), "Could not create sampler state.");
	}

	void Texture2D::Bind() const
	{
		RHI::Context->PSSetSamplers(0, 1, &Sampler);
		RHI::Context->PSSetShaderResources(0, 1, &Resource);
	}
}
