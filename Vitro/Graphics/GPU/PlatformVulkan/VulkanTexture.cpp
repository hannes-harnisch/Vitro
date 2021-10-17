module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.Texture;

import vt.Graphics.TextureSpecification;

namespace vt::vulkan
{
	export VkFormat convert_image_format(ImageFormat format)
	{
		using enum ImageFormat;
		switch(format)
		{ // clang-format off
			case Unknown:				return VK_FORMAT_UNDEFINED;
			case R32_G32_B32_A32_Float:	return VK_FORMAT_R32G32B32A32_SFLOAT;
			case R32_G32_B32_A32_UInt:	return VK_FORMAT_R32G32B32A32_UINT;
			case R32_G32_B32_A32_SInt:	return VK_FORMAT_R32G32B32A32_SINT;
			case R32_G32_B32_Float:		return VK_FORMAT_R32G32B32_SFLOAT;
			case R32_G32_B32_UInt:		return VK_FORMAT_R32G32B32_UINT;
			case R32_G32_B32_SInt:		return VK_FORMAT_R32G32B32_SINT;
			case R16_G16_B16_A16_Float:	return VK_FORMAT_R16G16B16A16_SFLOAT;
			case R16_G16_B16_A16_UNorm:	return VK_FORMAT_R16G16B16A16_UNORM;
			case R16_G16_B16_A16_UInt:	return VK_FORMAT_R16G16B16A16_UINT;
			case R16_G16_B16_A16_SNorm:	return VK_FORMAT_R16G16B16A16_SNORM;
			case R16_G16_B16_A16_SInt:	return VK_FORMAT_R16G16B16A16_SINT;
			case R32_G32_Float:			return VK_FORMAT_R32G32_SFLOAT;
			case R32_G32_UInt:			return VK_FORMAT_R32G32_UINT;
			case R32_G32_SInt:			return VK_FORMAT_R32G32_SINT;
			case R32_G8_X24_Typeless:
			case D32_Float_S8_X24_UInt:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
			case R10_G10_B10_A2_UNorm:	return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			case R10_G10_B10_A2_UInt:	return VK_FORMAT_A2B10G10R10_UINT_PACK32;
			case R11_G11_B10_Float:		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
			case R8_G8_B8_A8_UNorm:		return VK_FORMAT_R8G8B8A8_UNORM;
			case R8_G8_B8_A8_UNormSrgb:	return VK_FORMAT_R8G8B8A8_SRGB;
			case R8_G8_B8_A8_UInt:		return VK_FORMAT_R8G8B8A8_UINT;
			case R8_G8_B8_A8_SNorm:		return VK_FORMAT_R8G8B8A8_SNORM;
			case R8_G8_B8_A8_SInt:		return VK_FORMAT_R8G8B8A8_SINT;
			case B8_G8_R8_A8_UNorm:		return VK_FORMAT_B8G8R8A8_UNORM;
			case B8_G8_R8_A8_UNormSrgb:	return VK_FORMAT_B8G8R8A8_SRGB;
			case R16_G16_Float:			return VK_FORMAT_R16G16_SFLOAT;
			case R16_G16_UNorm:			return VK_FORMAT_R16G16_UNORM;
			case R16_G16_UInt:			return VK_FORMAT_R16G16_UINT;
			case R16_G16_SNorm:			return VK_FORMAT_R16G16_SNORM;
			case R16_G16_SInt:			return VK_FORMAT_R16G16_SINT;
			case R32_Typeless:			return VK_FORMAT_D32_SFLOAT;
			case D32_Float:				return VK_FORMAT_D32_SFLOAT;
			case R32_Float:				return VK_FORMAT_R32_SFLOAT;
			case R32_UInt:				return VK_FORMAT_R32_UINT;
			case R32_SInt:				return VK_FORMAT_R32_SINT;
			case R24_G8_Typeless:		return VK_FORMAT_D24_UNORM_S8_UINT;
			case D24_UNorm_S8_UInt:		return VK_FORMAT_D24_UNORM_S8_UINT;
			case R8_G8_UNorm:			return VK_FORMAT_R8G8_UNORM;
			case R8_G8_UInt:			return VK_FORMAT_R8G8_UINT;
			case R8_G8_SNorm:			return VK_FORMAT_R8G8_SNORM;
			case R8_G8_SInt:			return VK_FORMAT_R8G8_SINT;
			case R16_Typeless:			return VK_FORMAT_D16_UNORM;
			case R16_Float:				return VK_FORMAT_R16_SFLOAT;
			case D16_UNorm:				return VK_FORMAT_D16_UNORM;
			case R16_UNorm:				return VK_FORMAT_R16_UNORM;
			case R16_UInt:				return VK_FORMAT_R16_UINT;
			case R16_SNorm:				return VK_FORMAT_R16_SNORM;
			case R16_SInt:				return VK_FORMAT_R16_SINT;
			case R8_UNorm:				return VK_FORMAT_R8_UNORM;
			case R8_UInt:				return VK_FORMAT_R8_UINT;
			case R8_SNorm:				return VK_FORMAT_R8_SNORM;
			case R8_SInt:				return VK_FORMAT_R8_SINT;
			case Bc1_UNorm:				return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case Bc1_UNormSrgb:			return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
			case Bc2_UNorm:				return VK_FORMAT_BC2_UNORM_BLOCK;
			case Bc2_UNormSrgb:			return VK_FORMAT_BC2_SRGB_BLOCK;
			case Bc3_UNorm:				return VK_FORMAT_BC3_UNORM_BLOCK;
			case Bc3_UNormSrgb:			return VK_FORMAT_BC3_SRGB_BLOCK;
			case Bc4_UNorm:				return VK_FORMAT_BC4_UNORM_BLOCK;
			case Bc4_SNorm:				return VK_FORMAT_BC4_SNORM_BLOCK;
			case Bc5_UNorm:				return VK_FORMAT_BC5_UNORM_BLOCK;
			case Bc5_SNorm:				return VK_FORMAT_BC5_SNORM_BLOCK;
			case Bc6H_UFloat16:			return VK_FORMAT_BC6H_UFLOAT_BLOCK;
			case Bc6H_SFloat16:			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
			case Bc7_UNorm:				return VK_FORMAT_BC7_UNORM_BLOCK;
			case Bc7_UNormSrgb:			return VK_FORMAT_BC7_SRGB_BLOCK;
		}
		VT_UNREACHABLE();
	}

	export VkImageLayout convert_image_layout(ImageLayout layout)
	{
		using enum ImageLayout;
		switch(layout)
		{
			case Undefined:					return VK_IMAGE_LAYOUT_UNDEFINED;
			case General:
			case UnorderedAccess:			return VK_IMAGE_LAYOUT_GENERAL;
			case CopySource:				return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case CopyTarget:				return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			case ColorAttachment:			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case DepthStencilAttachment:	return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			case DepthStencilReadOnly:		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			case ShaderResource:
			case FragmentShaderResource:
			case NonFragmentShaderResource:	return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case Presentable:				return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		} // clang-format on
		VT_UNREACHABLE();
	}

	export class VulkanTexture
	{
	public:
	private:
	};
}
