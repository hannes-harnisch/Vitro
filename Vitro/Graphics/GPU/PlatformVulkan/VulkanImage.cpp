module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
export module vt.Graphics.Vulkan.Image;

import vt.Core.LookupTable;
import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.Vulkan.Handle;

namespace vt::vulkan
{
	export constexpr inline auto IMAGE_FORMAT_LOOKUP = [] {
		LookupTable<ImageFormat, VkFormat> _;
		using enum ImageFormat;

		_[Unknown]			 = VK_FORMAT_UNDEFINED;
		_[Rgba32Float]		 = VK_FORMAT_R32G32B32A32_SFLOAT;
		_[Rgba32UInt]		 = VK_FORMAT_R32G32B32A32_UINT;
		_[Rgba32SInt]		 = VK_FORMAT_R32G32B32A32_SINT;
		_[Rgb32Float]		 = VK_FORMAT_R32G32B32_SFLOAT;
		_[Rgb32UInt]		 = VK_FORMAT_R32G32B32_UINT;
		_[Rgb32SInt]		 = VK_FORMAT_R32G32B32_SINT;
		_[Rgba16Float]		 = VK_FORMAT_R16G16B16A16_SFLOAT;
		_[Rgba16UNorm]		 = VK_FORMAT_R16G16B16A16_UNORM;
		_[Rgba16UInt]		 = VK_FORMAT_R16G16B16A16_UINT;
		_[Rgba16SNorm]		 = VK_FORMAT_R16G16B16A16_SNORM;
		_[Rgba16SInt]		 = VK_FORMAT_R16G16B16A16_SINT;
		_[Rg32Float]		 = VK_FORMAT_R32G32_SFLOAT;
		_[Rg32UInt]			 = VK_FORMAT_R32G32_UINT;
		_[Rg32SInt]			 = VK_FORMAT_R32G32_SINT;
		_[R32G8X24Typeless]	 = VK_FORMAT_D32_SFLOAT_S8_UINT;
		_[D32FloatS8X24UInt] = VK_FORMAT_D32_SFLOAT_S8_UINT;
		_[Rgb10A2UNorm]		 = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		_[Rgb10A2UInt]		 = VK_FORMAT_A2B10G10R10_UINT_PACK32;
		_[Rg11B10Float]		 = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		_[Rgba8UNorm]		 = VK_FORMAT_R8G8B8A8_UNORM;
		_[Rgba8UNormSrgb]	 = VK_FORMAT_R8G8B8A8_SRGB;
		_[Rgba8UInt]		 = VK_FORMAT_R8G8B8A8_UINT;
		_[Rgba8SNorm]		 = VK_FORMAT_R8G8B8A8_SNORM;
		_[Rgba8SInt]		 = VK_FORMAT_R8G8B8A8_SINT;
		_[Bgra8UNorm]		 = VK_FORMAT_B8G8R8A8_UNORM;
		_[Bgra8UNormSrgb]	 = VK_FORMAT_B8G8R8A8_SRGB;
		_[Rg16Float]		 = VK_FORMAT_R16G16_SFLOAT;
		_[Rg16UNorm]		 = VK_FORMAT_R16G16_UNORM;
		_[Rg16UInt]			 = VK_FORMAT_R16G16_UINT;
		_[Rg16SNorm]		 = VK_FORMAT_R16G16_SNORM;
		_[Rg16SInt]			 = VK_FORMAT_R16G16_SINT;
		_[R32Typeless]		 = VK_FORMAT_R32_SFLOAT;
		_[D32Float]			 = VK_FORMAT_D32_SFLOAT;
		_[R32Float]			 = VK_FORMAT_R32_SFLOAT;
		_[R32UInt]			 = VK_FORMAT_R32_UINT;
		_[R32SInt]			 = VK_FORMAT_R32_SINT;
		_[R24G8Typeless]	 = VK_FORMAT_D24_UNORM_S8_UINT;
		_[D24UNormS8UInt]	 = VK_FORMAT_D24_UNORM_S8_UINT;
		_[Rg8UNorm]			 = VK_FORMAT_R8G8_UNORM;
		_[Rg8UInt]			 = VK_FORMAT_R8G8_UINT;
		_[Rg8SNorm]			 = VK_FORMAT_R8G8_SNORM;
		_[Rg8SInt]			 = VK_FORMAT_R8G8_SINT;
		_[R16Typeless]		 = VK_FORMAT_D16_UNORM;
		_[R16Float]			 = VK_FORMAT_R16_SFLOAT;
		_[D16UNorm]			 = VK_FORMAT_D16_UNORM;
		_[R16UNorm]			 = VK_FORMAT_R16_UNORM;
		_[R16UInt]			 = VK_FORMAT_R16_UINT;
		_[R16SNorm]			 = VK_FORMAT_R16_SNORM;
		_[R16SInt]			 = VK_FORMAT_R16_SINT;
		_[R8UNorm]			 = VK_FORMAT_R8_UNORM;
		_[R8UInt]			 = VK_FORMAT_R8_UINT;
		_[R8SNorm]			 = VK_FORMAT_R8_SNORM;
		_[R8SInt]			 = VK_FORMAT_R8_SINT;
		_[Bc1UNorm]			 = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		_[Bc1UNormSrgb]		 = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
		_[Bc2UNorm]			 = VK_FORMAT_BC2_UNORM_BLOCK;
		_[Bc2UNormSrgb]		 = VK_FORMAT_BC2_SRGB_BLOCK;
		_[Bc3UNorm]			 = VK_FORMAT_BC3_UNORM_BLOCK;
		_[Bc3UNormSrgb]		 = VK_FORMAT_BC3_SRGB_BLOCK;
		_[Bc4UNorm]			 = VK_FORMAT_BC4_UNORM_BLOCK;
		_[Bc4SNorm]			 = VK_FORMAT_BC4_SNORM_BLOCK;
		_[Bc5UNorm]			 = VK_FORMAT_BC5_UNORM_BLOCK;
		_[Bc5SNorm]			 = VK_FORMAT_BC5_SNORM_BLOCK;
		_[Bc6HUFloat16]		 = VK_FORMAT_BC6H_UFLOAT_BLOCK;
		_[Bc6HSFloat16]		 = VK_FORMAT_BC6H_SFLOAT_BLOCK;
		_[Bc7UNorm]			 = VK_FORMAT_BC7_UNORM_BLOCK;
		_[Bc7UNormSrgb]		 = VK_FORMAT_BC7_SRGB_BLOCK;
		return _;
	}();

	export constexpr inline auto VULKAN_FORMAT_LOOKUP = [] {
		constexpr size_t MAX = VK_FORMAT_BC7_SRGB_BLOCK + 1;

		LookupTable<VkFormat, ImageFormat, MAX> _;
		using enum ImageFormat;

		_[VK_FORMAT_UNDEFINED]				  = Unknown;
		_[VK_FORMAT_R32G32B32A32_SFLOAT]	  = Rgba32Float;
		_[VK_FORMAT_R32G32B32A32_UINT]		  = Rgba32UInt;
		_[VK_FORMAT_R32G32B32A32_SINT]		  = Rgba32SInt;
		_[VK_FORMAT_R32G32B32_SFLOAT]		  = Rgb32Float;
		_[VK_FORMAT_R32G32B32_UINT]			  = Rgb32UInt;
		_[VK_FORMAT_R32G32B32_SINT]			  = Rgb32SInt;
		_[VK_FORMAT_R16G16B16A16_SFLOAT]	  = Rgba16Float;
		_[VK_FORMAT_R16G16B16A16_UNORM]		  = Rgba16UNorm;
		_[VK_FORMAT_R16G16B16A16_UINT]		  = Rgba16UInt;
		_[VK_FORMAT_R16G16B16A16_SNORM]		  = Rgba16SNorm;
		_[VK_FORMAT_R16G16B16A16_SINT]		  = Rgba16SInt;
		_[VK_FORMAT_R32G32_SFLOAT]			  = Rg32Float;
		_[VK_FORMAT_R32G32_UINT]			  = Rg32UInt;
		_[VK_FORMAT_R32G32_SINT]			  = Rg32SInt;
		_[VK_FORMAT_D32_SFLOAT_S8_UINT]		  = D32FloatS8X24UInt;
		_[VK_FORMAT_A2B10G10R10_UNORM_PACK32] = Rgb10A2UNorm;
		_[VK_FORMAT_A2B10G10R10_UINT_PACK32]  = Rgb10A2UInt;
		_[VK_FORMAT_B10G11R11_UFLOAT_PACK32]  = Rg11B10Float;
		_[VK_FORMAT_R8G8B8A8_UNORM]			  = Rgba8UNorm;
		_[VK_FORMAT_R8G8B8A8_SRGB]			  = Rgba8UNormSrgb;
		_[VK_FORMAT_R8G8B8A8_UINT]			  = Rgba8UInt;
		_[VK_FORMAT_R8G8B8A8_SNORM]			  = Rgba8SNorm;
		_[VK_FORMAT_R8G8B8A8_SINT]			  = Rgba8SInt;
		_[VK_FORMAT_B8G8R8A8_UNORM]			  = Bgra8UNorm;
		_[VK_FORMAT_B8G8R8A8_SRGB]			  = Bgra8UNormSrgb;
		_[VK_FORMAT_R16G16_SFLOAT]			  = Rg16Float;
		_[VK_FORMAT_R16G16_UNORM]			  = Rg16UNorm;
		_[VK_FORMAT_R16G16_UINT]			  = Rg16UInt;
		_[VK_FORMAT_R16G16_SNORM]			  = Rg16SNorm;
		_[VK_FORMAT_R16G16_SINT]			  = Rg16SInt;
		_[VK_FORMAT_D32_SFLOAT]				  = D32Float;
		_[VK_FORMAT_R32_SFLOAT]				  = R32Float;
		_[VK_FORMAT_R32_UINT]				  = R32UInt;
		_[VK_FORMAT_R32_SINT]				  = R32SInt;
		_[VK_FORMAT_D24_UNORM_S8_UINT]		  = D24UNormS8UInt;
		_[VK_FORMAT_R8G8_UNORM]				  = Rg8UNorm;
		_[VK_FORMAT_R8G8_UINT]				  = Rg8UInt;
		_[VK_FORMAT_R8G8_SNORM]				  = Rg8SNorm;
		_[VK_FORMAT_R8G8_SINT]				  = Rg8SInt;
		_[VK_FORMAT_R16_SFLOAT]				  = R16Float;
		_[VK_FORMAT_D16_UNORM]				  = D16UNorm;
		_[VK_FORMAT_R16_UNORM]				  = R16UNorm;
		_[VK_FORMAT_R16_UINT]				  = R16UInt;
		_[VK_FORMAT_R16_SNORM]				  = R16SNorm;
		_[VK_FORMAT_R16_SINT]				  = R16SInt;
		_[VK_FORMAT_R8_UNORM]				  = R8UNorm;
		_[VK_FORMAT_R8_UINT]				  = R8UInt;
		_[VK_FORMAT_R8_SNORM]				  = R8SNorm;
		_[VK_FORMAT_R8_SINT]				  = R8SInt;
		_[VK_FORMAT_BC1_RGBA_UNORM_BLOCK]	  = Bc1UNorm;
		_[VK_FORMAT_BC1_RGBA_SRGB_BLOCK]	  = Bc1UNormSrgb;
		_[VK_FORMAT_BC2_UNORM_BLOCK]		  = Bc2UNorm;
		_[VK_FORMAT_BC2_SRGB_BLOCK]			  = Bc2UNormSrgb;
		_[VK_FORMAT_BC3_UNORM_BLOCK]		  = Bc3UNorm;
		_[VK_FORMAT_BC3_SRGB_BLOCK]			  = Bc3UNormSrgb;
		_[VK_FORMAT_BC4_UNORM_BLOCK]		  = Bc4UNorm;
		_[VK_FORMAT_BC4_SNORM_BLOCK]		  = Bc4SNorm;
		_[VK_FORMAT_BC5_UNORM_BLOCK]		  = Bc5UNorm;
		_[VK_FORMAT_BC5_SNORM_BLOCK]		  = Bc5SNorm;
		_[VK_FORMAT_BC6H_UFLOAT_BLOCK]		  = Bc6HUFloat16;
		_[VK_FORMAT_BC6H_SFLOAT_BLOCK]		  = Bc6HSFloat16;
		_[VK_FORMAT_BC7_UNORM_BLOCK]		  = Bc7UNorm;
		_[VK_FORMAT_BC7_SRGB_BLOCK]			  = Bc7UNormSrgb;
		return _;
	}();

	export constexpr inline auto IMAGE_LAYOUT_LOOKUP = [] {
		LookupTable<ImageLayout, VkImageLayout> _;
		using enum ImageLayout;

		_[Undefined]			  = VK_IMAGE_LAYOUT_UNDEFINED;
		_[General]				  = VK_IMAGE_LAYOUT_GENERAL;
		_[UnorderedAccess]		  = VK_IMAGE_LAYOUT_GENERAL;
		_[CopySource]			  = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		_[CopyDestination]		  = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		_[ColorAttachment]		  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		_[DepthStencilAttachment] = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		_[DepthStencilReadOnly]	  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		_[ShaderResource]		  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		_[FragmentShaderResource] = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		_[Presentable]			  = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return _;
	}();

	constexpr inline auto IMAGE_DIMENSION_LOOKUP = [] {
		LookupTable<ImageDimension, VkImageType> _;
		using enum ImageDimension;

		_[Image1D]	 = VK_IMAGE_TYPE_1D;
		_[Image2D]	 = VK_IMAGE_TYPE_2D;
		_[Image3D]	 = VK_IMAGE_TYPE_3D;
		_[Cube]		 = VK_IMAGE_TYPE_2D;
		_[Array1D]	 = VK_IMAGE_TYPE_1D;
		_[Array2D]	 = VK_IMAGE_TYPE_2D;
		_[CubeArray] = VK_IMAGE_TYPE_2D;
		return _;
	}();

	VkImageCreateFlags derive_image_creation_flags(ImageDimension dimension)
	{
		if(dimension == ImageDimension::Cube || dimension == ImageDimension::CubeArray)
			return VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		else
			return 0;
	}

	VkImageUsageFlags convert_image_usage(ImageUsage usage)
	{
		VkImageUsageFlags flags = 0;

		using enum ImageUsage;
		if(usage & CopySrc)
			flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if(usage & CopyDst)
			flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if(usage & Sampled)
			flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		if(usage & Storage)
			flags |= VK_IMAGE_USAGE_STORAGE_BIT;
		if(usage & ColorAttachment)
			flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if(usage & DepthStencil)
			flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if(usage & Transient)
			flags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		if(usage & InputAttachment)
			flags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

		return flags;
	}

	constexpr inline auto IMAGE_VIEW_TYPE_LOOKUP = [] {
		LookupTable<ImageDimension, VkImageViewType> _;
		using enum ImageDimension;

		_[Image1D]	 = VK_IMAGE_VIEW_TYPE_1D;
		_[Image2D]	 = VK_IMAGE_VIEW_TYPE_2D;
		_[Image3D]	 = VK_IMAGE_VIEW_TYPE_3D;
		_[Cube]		 = VK_IMAGE_VIEW_TYPE_CUBE;
		_[Array1D]	 = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
		_[Array2D]	 = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		_[CubeArray] = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		return _;
	}();

	export constexpr inline auto IMAGE_ASPECT_FLAGS_LOOKUP = [] {
		LookupTable<ImageFormat, VkImageAspectFlags> _;
		using enum ImageFormat;

		constexpr VkImageAspectFlags COLOR = VK_IMAGE_ASPECT_COLOR_BIT;

		_[Unknown]			 = 0;
		_[Rgba32Float]		 = COLOR;
		_[Rgba32UInt]		 = COLOR;
		_[Rgba32SInt]		 = COLOR;
		_[Rgb32Float]		 = COLOR;
		_[Rgb32UInt]		 = COLOR;
		_[Rgb32SInt]		 = COLOR;
		_[Rgba16Float]		 = COLOR;
		_[Rgba16UNorm]		 = COLOR;
		_[Rgba16UInt]		 = COLOR;
		_[Rgba16SNorm]		 = COLOR;
		_[Rgba16SInt]		 = COLOR;
		_[Rg32Float]		 = COLOR;
		_[Rg32UInt]			 = COLOR;
		_[Rg32SInt]			 = COLOR;
		_[R32G8X24Typeless]	 = COLOR;
		_[D32FloatS8X24UInt] = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		_[Rgb10A2UNorm]		 = COLOR;
		_[Rgb10A2UInt]		 = COLOR;
		_[Rg11B10Float]		 = COLOR;
		_[Rgba8UNorm]		 = COLOR;
		_[Rgba8UNormSrgb]	 = COLOR;
		_[Rgba8UInt]		 = COLOR;
		_[Rgba8SNorm]		 = COLOR;
		_[Rgba8SInt]		 = COLOR;
		_[Bgra8UNorm]		 = COLOR;
		_[Bgra8UNormSrgb]	 = COLOR;
		_[Rg16Float]		 = COLOR;
		_[Rg16UNorm]		 = COLOR;
		_[Rg16UInt]			 = COLOR;
		_[Rg16SNorm]		 = COLOR;
		_[Rg16SInt]			 = COLOR;
		_[R32Typeless]		 = COLOR;
		_[D32Float]			 = VK_IMAGE_ASPECT_DEPTH_BIT;
		_[R32Float]			 = COLOR;
		_[R32UInt]			 = COLOR;
		_[R32SInt]			 = COLOR;
		_[R24G8Typeless]	 = COLOR;
		_[D24UNormS8UInt]	 = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		_[Rg8UNorm]			 = COLOR;
		_[Rg8UInt]			 = COLOR;
		_[Rg8SNorm]			 = COLOR;
		_[Rg8SInt]			 = COLOR;
		_[R16Typeless]		 = COLOR;
		_[R16Float]			 = COLOR;
		_[D16UNorm]			 = VK_IMAGE_ASPECT_DEPTH_BIT;
		_[R16UNorm]			 = COLOR;
		_[R16UInt]			 = COLOR;
		_[R16SNorm]			 = COLOR;
		_[R16SInt]			 = COLOR;
		_[R8UNorm]			 = COLOR;
		_[R8UInt]			 = COLOR;
		_[R8SNorm]			 = COLOR;
		_[R8SInt]			 = COLOR;
		_[Bc1UNorm]			 = COLOR;
		_[Bc1UNormSrgb]		 = COLOR;
		_[Bc2UNorm]			 = COLOR;
		_[Bc2UNormSrgb]		 = COLOR;
		_[Bc3UNorm]			 = COLOR;
		_[Bc3UNormSrgb]		 = COLOR;
		_[Bc4UNorm]			 = COLOR;
		_[Bc4SNorm]			 = COLOR;
		_[Bc5UNorm]			 = COLOR;
		_[Bc5SNorm]			 = COLOR;
		_[Bc6HUFloat16]		 = COLOR;
		_[Bc6HSFloat16]		 = COLOR;
		_[Bc7UNorm]			 = COLOR;
		_[Bc7UNormSrgb]		 = COLOR;
		return _;
	}();

	export class VulkanImage
	{
	public:
		VulkanImage(ImageSpecification const& spec, DeviceApiTable const& api, VmaAllocator allocator)
		{
			initialize_image(spec, allocator);
			image.get_deleter().api = &api;
			initialize_image_view(spec, api);
		}

		VkImage get_handle() const
		{
			return image.get();
		}

		VmaAllocation get_allocation() const
		{
			return image.get_deleter().allocation;
		}

		VkImageView get_view() const
		{
			return image.get_deleter().image_view;
		}

	private:
		struct ImageDeleter
		{
			using pointer = VkImage;

			DeviceApiTable const* api;
			VmaAllocation		  allocation;
			VkImageView			  image_view;

			void operator()(VkImage image) const
			{
				api->vkDestroyImageView(api->device, image_view, nullptr);
				vmaDestroyImage(api->allocator, image, allocation);
			}
		};
		using UniqueVkImage = std::unique_ptr<VkImage, ImageDeleter>;

		UniqueVkImage image;

		void initialize_image(ImageSpecification const& spec, VmaAllocator allocator)
		{
			VmaAllocationCreateInfo const allocation_info {
				.flags			= 0,
				.usage			= VMA_MEMORY_USAGE_GPU_ONLY,
				.requiredFlags	= 0,
				.preferredFlags = 0,
				.memoryTypeBits = 0,
				.pool			= nullptr,
				.pUserData		= nullptr,
				.priority		= 0,
			};
			VkImageCreateInfo const image_info {
				.sType	   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.flags	   = derive_image_creation_flags(spec.dimension),
				.imageType = IMAGE_DIMENSION_LOOKUP[spec.dimension],
				.format	   = IMAGE_FORMAT_LOOKUP[spec.format],
				.extent {
					.width	= spec.expanse.width,
					.height = spec.expanse.height,
					.depth	= spec.expanse.depth,
				},
				.mipLevels			   = spec.mip_count,
				.arrayLayers		   = spec.expanse.depth,
				.samples			   = static_cast<VkSampleCountFlagBits>(spec.sample_count),
				.tiling				   = VK_IMAGE_TILING_OPTIMAL,
				.usage				   = convert_image_usage(spec.usage),
				.sharingMode		   = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 0,
				.pQueueFamilyIndices   = nullptr,
				.initialLayout		   = VK_IMAGE_LAYOUT_UNDEFINED,
			};
			auto result = vmaCreateImage(allocator, &image_info, &allocation_info, std::out_ptr(image),
										 &image.get_deleter().allocation, nullptr);
			VT_CHECK_RESULT(result, "Failed to create Vulkan image.");
		}

		void initialize_image_view(ImageSpecification const& spec, DeviceApiTable const& api)
		{
			VkImageViewCreateInfo const image_view_info {
				.sType	  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image	  = image.get(),
				.viewType = IMAGE_VIEW_TYPE_LOOKUP[spec.dimension],
				.format	  = IMAGE_FORMAT_LOOKUP[spec.format],
				.components {
					.r = VK_COMPONENT_SWIZZLE_IDENTITY,
					.g = VK_COMPONENT_SWIZZLE_IDENTITY,
					.b = VK_COMPONENT_SWIZZLE_IDENTITY,
					.a = VK_COMPONENT_SWIZZLE_IDENTITY,
				},
				.subresourceRange {
					.aspectMask		= IMAGE_ASPECT_FLAGS_LOOKUP[spec.format],
					.baseMipLevel	= 0,
					.levelCount		= spec.mip_count,
					.baseArrayLayer = 0,
					.layerCount		= spec.expanse.depth,
				},
			};
			auto result = api.vkCreateImageView(api.device, &image_view_info, nullptr, &image.get_deleter().image_view);
			VT_CHECK_RESULT(result, "Failed to create Vulkan image view.");
		}
	};
}
