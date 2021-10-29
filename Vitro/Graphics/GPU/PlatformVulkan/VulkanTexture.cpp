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
			case Unknown:			return VK_FORMAT_UNDEFINED;
			case Rgba32Float:		return VK_FORMAT_R32G32B32A32_SFLOAT;
			case Rgba32UInt:		return VK_FORMAT_R32G32B32A32_UINT;
			case Rgba32SInt:		return VK_FORMAT_R32G32B32A32_SINT;
			case Rgb32Float:		return VK_FORMAT_R32G32B32_SFLOAT;
			case Rgb32UInt:			return VK_FORMAT_R32G32B32_UINT;
			case Rgb32SInt:			return VK_FORMAT_R32G32B32_SINT;
			case Rgba16Float:		return VK_FORMAT_R16G16B16A16_SFLOAT;
			case Rgba16UNorm:		return VK_FORMAT_R16G16B16A16_UNORM;
			case Rgba16UInt:		return VK_FORMAT_R16G16B16A16_UINT;
			case Rgba16SNorm:		return VK_FORMAT_R16G16B16A16_SNORM;
			case Rgba16SInt:		return VK_FORMAT_R16G16B16A16_SINT;
			case Rg32Float:			return VK_FORMAT_R32G32_SFLOAT;
			case Rg32UInt:			return VK_FORMAT_R32G32_UINT;
			case Rg32SInt:			return VK_FORMAT_R32G32_SINT;
			case R32G8X24Typeless:
			case D32FloatS8X24UInt:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
			case Rgb10A2UNorm:		return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			case Rgb10A2UInt:		return VK_FORMAT_A2B10G10R10_UINT_PACK32;
			case Rg11B10Float:		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
			case Rgba8UNorm:		return VK_FORMAT_R8G8B8A8_UNORM;
			case Rgba8UNormSrgb:	return VK_FORMAT_R8G8B8A8_SRGB;
			case Rgba8UInt:			return VK_FORMAT_R8G8B8A8_UINT;
			case Rgba8SNorm:		return VK_FORMAT_R8G8B8A8_SNORM;
			case Rgba8SInt:			return VK_FORMAT_R8G8B8A8_SINT;
			case Bgra8UNorm:		return VK_FORMAT_B8G8R8A8_UNORM;
			case Bgra8UNormSrgb:	return VK_FORMAT_B8G8R8A8_SRGB;
			case Rg16Float:			return VK_FORMAT_R16G16_SFLOAT;
			case Rg16UNorm:			return VK_FORMAT_R16G16_UNORM;
			case Rg16UInt:			return VK_FORMAT_R16G16_UINT;
			case Rg16SNorm:			return VK_FORMAT_R16G16_SNORM;
			case Rg16SInt:			return VK_FORMAT_R16G16_SINT;
			case R32Typeless:		return VK_FORMAT_D32_SFLOAT;
			case D32Float:			return VK_FORMAT_D32_SFLOAT;
			case R32Float:			return VK_FORMAT_R32_SFLOAT;
			case R32UInt:			return VK_FORMAT_R32_UINT;
			case R32SInt:			return VK_FORMAT_R32_SINT;
			case R24G8Typeless:		return VK_FORMAT_D24_UNORM_S8_UINT;
			case D24UNormS8UInt:	return VK_FORMAT_D24_UNORM_S8_UINT;
			case Rg8UNorm:			return VK_FORMAT_R8G8_UNORM;
			case Rg8UInt:			return VK_FORMAT_R8G8_UINT;
			case Rg8SNorm:			return VK_FORMAT_R8G8_SNORM;
			case Rg8SInt:			return VK_FORMAT_R8G8_SINT;
			case R16Typeless:		return VK_FORMAT_D16_UNORM;
			case R16Float:			return VK_FORMAT_R16_SFLOAT;
			case D16UNorm:			return VK_FORMAT_D16_UNORM;
			case R16UNorm:			return VK_FORMAT_R16_UNORM;
			case R16UInt:			return VK_FORMAT_R16_UINT;
			case R16SNorm:			return VK_FORMAT_R16_SNORM;
			case R16SInt:			return VK_FORMAT_R16_SINT;
			case R8UNorm:			return VK_FORMAT_R8_UNORM;
			case R8UInt:			return VK_FORMAT_R8_UINT;
			case R8SNorm:			return VK_FORMAT_R8_SNORM;
			case R8SInt:			return VK_FORMAT_R8_SINT;
			case Bc1UNorm:			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case Bc1UNormSrgb:		return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
			case Bc2UNorm:			return VK_FORMAT_BC2_UNORM_BLOCK;
			case Bc2UNormSrgb:		return VK_FORMAT_BC2_SRGB_BLOCK;
			case Bc3UNorm:			return VK_FORMAT_BC3_UNORM_BLOCK;
			case Bc3UNormSrgb:		return VK_FORMAT_BC3_SRGB_BLOCK;
			case Bc4UNorm:			return VK_FORMAT_BC4_UNORM_BLOCK;
			case Bc4SNorm:			return VK_FORMAT_BC4_SNORM_BLOCK;
			case Bc5UNorm:			return VK_FORMAT_BC5_UNORM_BLOCK;
			case Bc5SNorm:			return VK_FORMAT_BC5_SNORM_BLOCK;
			case Bc6HUFloat16:		return VK_FORMAT_BC6H_UFLOAT_BLOCK;
			case Bc6HSFloat16:		return VK_FORMAT_BC6H_SFLOAT_BLOCK;
			case Bc7UNorm:			return VK_FORMAT_BC7_UNORM_BLOCK;
			case Bc7UNormSrgb:		return VK_FORMAT_BC7_SRGB_BLOCK;
		}
		VT_UNREACHABLE();
	}

	export ImageFormat canonicalize_image_format(VkFormat format)
	{
		using enum ImageFormat;
		switch (format)
		{
			case VK_FORMAT_UNDEFINED:				 return Unknown;
			case VK_FORMAT_R32G32B32A32_SFLOAT:		 return Rgba32Float;
			case VK_FORMAT_R32G32B32A32_UINT:		 return Rgba32UInt;
			case VK_FORMAT_R32G32B32A32_SINT:		 return Rgba32SInt;
			case VK_FORMAT_R32G32B32_SFLOAT:		 return Rgb32Float;
			case VK_FORMAT_R32G32B32_UINT:			 return Rgb32UInt;
			case VK_FORMAT_R32G32B32_SINT:			 return Rgb32SInt;
			case VK_FORMAT_R16G16B16A16_SFLOAT:		 return Rgba16Float;
			case VK_FORMAT_R16G16B16A16_UNORM:		 return Rgba16UNorm;
			case VK_FORMAT_R16G16B16A16_UINT:		 return Rgba16UInt;
			case VK_FORMAT_R16G16B16A16_SNORM:		 return Rgba16SNorm;
			case VK_FORMAT_R16G16B16A16_SINT:		 return Rgba16SInt;
			case VK_FORMAT_R32G32_SFLOAT:			 return Rg32Float;
			case VK_FORMAT_R32G32_UINT:				 return Rg32UInt;
			case VK_FORMAT_R32G32_SINT:				 return Rg32SInt;
			case VK_FORMAT_D32_SFLOAT_S8_UINT:		 return D32FloatS8X24UInt;
			case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return Rgb10A2UNorm;
			case VK_FORMAT_A2B10G10R10_UINT_PACK32:	 return Rgb10A2UInt;
			case VK_FORMAT_B10G11R11_UFLOAT_PACK32:	 return Rg11B10Float;
			case VK_FORMAT_R8G8B8A8_UNORM:			 return Rgba8UNorm;
			case VK_FORMAT_R8G8B8A8_SRGB:			 return Rgba8UNormSrgb;
			case VK_FORMAT_R8G8B8A8_UINT:			 return Rgba8UInt;
			case VK_FORMAT_R8G8B8A8_SNORM:			 return Rgba8SNorm;
			case VK_FORMAT_R8G8B8A8_SINT:			 return Rgba8SInt;
			case VK_FORMAT_B8G8R8A8_UNORM:			 return Bgra8UNorm;
			case VK_FORMAT_B8G8R8A8_SRGB:			 return Bgra8UNormSrgb;
			case VK_FORMAT_R16G16_SFLOAT:			 return Rg16Float;
			case VK_FORMAT_R16G16_UNORM:			 return Rg16UNorm;
			case VK_FORMAT_R16G16_UINT:				 return Rg16UInt;
			case VK_FORMAT_R16G16_SNORM:			 return Rg16SNorm;
			case VK_FORMAT_R16G16_SINT:				 return Rg16SInt;
			case VK_FORMAT_D32_SFLOAT:				 return D32Float;
			case VK_FORMAT_R32_SFLOAT:				 return R32Float;
			case VK_FORMAT_R32_UINT:				 return R32UInt;
			case VK_FORMAT_R32_SINT:				 return R32SInt;
			case VK_FORMAT_D24_UNORM_S8_UINT:		 return D24UNormS8UInt;
			case VK_FORMAT_R8G8_UNORM:				 return Rg8UNorm;
			case VK_FORMAT_R8G8_UINT:				 return Rg8UInt;
			case VK_FORMAT_R8G8_SNORM:				 return Rg8SNorm;
			case VK_FORMAT_R8G8_SINT:				 return Rg8SInt;
			case VK_FORMAT_R16_SFLOAT:				 return R16Float;
			case VK_FORMAT_D16_UNORM:				 return D16UNorm;
			case VK_FORMAT_R16_UNORM:				 return R16UNorm;
			case VK_FORMAT_R16_UINT:				 return R16UInt;
			case VK_FORMAT_R16_SNORM:				 return R16SNorm;
			case VK_FORMAT_R16_SINT:				 return R16SInt;
			case VK_FORMAT_R8_UNORM:				 return R8UNorm;
			case VK_FORMAT_R8_UINT:					 return R8UInt;
			case VK_FORMAT_R8_SNORM:				 return R8SNorm;
			case VK_FORMAT_R8_SINT:					 return R8SInt;
			case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:	 return Bc1UNorm;
			case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:		 return Bc1UNormSrgb;
			case VK_FORMAT_BC2_UNORM_BLOCK:			 return Bc2UNorm;
			case VK_FORMAT_BC2_SRGB_BLOCK:			 return Bc2UNormSrgb;
			case VK_FORMAT_BC3_UNORM_BLOCK:			 return Bc3UNorm;
			case VK_FORMAT_BC3_SRGB_BLOCK:			 return Bc3UNormSrgb;
			case VK_FORMAT_BC4_UNORM_BLOCK:			 return Bc4UNorm;
			case VK_FORMAT_BC4_SNORM_BLOCK:			 return Bc4SNorm;
			case VK_FORMAT_BC5_UNORM_BLOCK:			 return Bc5UNorm;
			case VK_FORMAT_BC5_SNORM_BLOCK:			 return Bc5SNorm;
			case VK_FORMAT_BC6H_UFLOAT_BLOCK:		 return Bc6HUFloat16;
			case VK_FORMAT_BC6H_SFLOAT_BLOCK:		 return Bc6HSFloat16;
			case VK_FORMAT_BC7_UNORM_BLOCK:			 return Bc7UNorm;
			case VK_FORMAT_BC7_SRGB_BLOCK:			 return Bc7UNormSrgb;
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
		VkImage get_image() const
		{
			return image;
		}

		VkImageView get_image_view() const
		{
			return image_view;
		}

	private:
		VkImage		image;
		VkImageView image_view;
	};
}
