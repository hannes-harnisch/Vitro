module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module Vitro.D3D12.Texture;

import Vitro.D3D12.Utils;
import Vitro.Graphics.Device;
import Vitro.Graphics.TextureInfo;

namespace vt::d3d12
{
	export constexpr DXGI_FORMAT convertImageFormat(ImageFormat format)
	{
		using enum ImageFormat;
		switch(format)
		{
			case Unknown: return DXGI_FORMAT_UNKNOWN;
			case R32_G32_B32_A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case R32_G32_B32_A32_UInt: return DXGI_FORMAT_R32G32B32A32_UINT;
			case R32_G32_B32_A32_SInt: return DXGI_FORMAT_R32G32B32A32_SINT;
			case R32_G32_B32_Float: return DXGI_FORMAT_R32G32B32_FLOAT;
			case R32_G32_B32_UInt: return DXGI_FORMAT_R32G32B32_UINT;
			case R32_G32_B32_SInt: return DXGI_FORMAT_R32G32B32_SINT;
			case R16_G16_B16_A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case R16_G16_B16_A16_UNorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
			case R16_G16_B16_A16_UInt: return DXGI_FORMAT_R16G16B16A16_UINT;
			case R16_G16_B16_A16_SNorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
			case R16_G16_B16_A16_SInt: return DXGI_FORMAT_R16G16B16A16_SINT;
			case R32_G32_Float: return DXGI_FORMAT_R32G32_FLOAT;
			case R32_G32_UInt: return DXGI_FORMAT_R32G32_UINT;
			case R32_G32_SInt: return DXGI_FORMAT_R32G32_SINT;
			case R32_G8_X24_Typeless: return DXGI_FORMAT_R32G8X24_TYPELESS;
			case D32_Float_S8_X24_UInt: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			case R10_G10_B10_A2_UNorm: return DXGI_FORMAT_R10G10B10A2_UNORM;
			case R10_G10_B10_A2_UInt: return DXGI_FORMAT_R10G10B10A2_UINT;
			case R11_G11_B10_Float: return DXGI_FORMAT_R11G11B10_FLOAT;
			case R8_G8_B8_A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case R8_G8_B8_A8_UNormSrgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case R8_G8_B8_A8_UInt: return DXGI_FORMAT_R8G8B8A8_UINT;
			case R8_G8_B8_A8_SNorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
			case R8_G8_B8_A8_SInt: return DXGI_FORMAT_R8G8B8A8_SINT;
			case B8_G8_R8_A8_UNorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
			case B8_G8_R8_A8_UNormSrgb: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			case R16_G16_Float: return DXGI_FORMAT_R16G16_FLOAT;
			case R16_G16_UNorm: return DXGI_FORMAT_R16G16_UNORM;
			case R16_G16_UInt: return DXGI_FORMAT_R16G16_UINT;
			case R16_G16_SNorm: return DXGI_FORMAT_R16G16_SNORM;
			case R16_G16_SInt: return DXGI_FORMAT_R16G16_SINT;
			case R32_Typeless: return DXGI_FORMAT_R32_TYPELESS;
			case D32_Float: return DXGI_FORMAT_D32_FLOAT;
			case R32_Float: return DXGI_FORMAT_R32_FLOAT;
			case R32_UInt: return DXGI_FORMAT_R32_UINT;
			case R32_SInt: return DXGI_FORMAT_R32_SINT;
			case R24_G8_Typeless: return DXGI_FORMAT_R24G8_TYPELESS;
			case D24_UNorm_S8_UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case R8_G8_UNorm: return DXGI_FORMAT_R8G8_UNORM;
			case R8_G8_UInt: return DXGI_FORMAT_R8G8_UINT;
			case R8_G8_SNorm: return DXGI_FORMAT_R8G8_SNORM;
			case R8_G8_SInt: return DXGI_FORMAT_R8G8_SINT;
			case R16_Typeless: return DXGI_FORMAT_R16_TYPELESS;
			case R16_Float: return DXGI_FORMAT_R16_FLOAT;
			case D16_UNorm: return DXGI_FORMAT_D16_UNORM;
			case R16_UNorm: return DXGI_FORMAT_R16_UNORM;
			case R16_UInt: return DXGI_FORMAT_R16_UINT;
			case R16_SNorm: return DXGI_FORMAT_R16_SNORM;
			case R16_SInt: return DXGI_FORMAT_R16_SINT;
			case R8_UNorm: return DXGI_FORMAT_R8_UNORM;
			case R8_UInt: return DXGI_FORMAT_R8_UINT;
			case R8_SNorm: return DXGI_FORMAT_R8_SNORM;
			case R8_SInt: return DXGI_FORMAT_R8_SINT;
			case Bc1_UNorm: return DXGI_FORMAT_BC1_UNORM;
			case Bc1_UNormSrgb: return DXGI_FORMAT_BC1_UNORM_SRGB;
			case Bc2_UNorm: return DXGI_FORMAT_BC2_UNORM;
			case Bc2_UNormSrgb: return DXGI_FORMAT_BC2_UNORM_SRGB;
			case Bc3_UNorm: return DXGI_FORMAT_BC3_UNORM;
			case Bc3_UNormSrgb: return DXGI_FORMAT_BC3_UNORM_SRGB;
			case Bc4_UNorm: return DXGI_FORMAT_BC4_UNORM;
			case Bc4_SNorm: return DXGI_FORMAT_BC4_SNORM;
			case Bc5_UNorm: return DXGI_FORMAT_BC5_UNORM;
			case Bc5_SNorm: return DXGI_FORMAT_BC5_SNORM;
			case Bc6H_UFloat16: return DXGI_FORMAT_BC6H_UF16;
			case Bc6H_SFloat16: return DXGI_FORMAT_BC6H_SF16;
			case Bc7_UNorm: return DXGI_FORMAT_BC7_UNORM;
			case Bc7_UNormSrgb: return DXGI_FORMAT_BC7_UNORM_SRGB;
		}
		vtUnreachable();
	}

	export constexpr D3D12_RESOURCE_STATES convertImageLayout(ImageLayout layout)
	{
		using enum ImageLayout;
		switch(layout)
		{
			case Undefined:
			case General:
			case Presentable: return D3D12_RESOURCE_STATE_COMMON;
			case CopySource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
			case CopyTarget: return D3D12_RESOURCE_STATE_COPY_DEST;
			case ColorAttachment: return D3D12_RESOURCE_STATE_RENDER_TARGET;
			case DepthStencilAttachment: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case DepthStencilReadOnly: return D3D12_RESOURCE_STATE_DEPTH_READ;
			case ShaderResource:
				return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			case FragmentShaderResource: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			case NonFragmentShaderResource: return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			case UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}
		vtUnreachable();
	}

	export class Texture
	{
	public:
		ID3D12Resource* get() const
		{
			return texture.get();
		}

	private:
		ComUnique<ID3D12Resource> texture;
	};
}
