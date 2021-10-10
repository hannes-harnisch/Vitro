module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.Sampler;

import vt.Graphics.Vulkan.Driver;
import vt.Graphics.DescriptorSetLayout;

namespace vt::vulkan
{
	VkFilter convert_mag_filter(Filter filter)
	{
		using enum Filter;
		switch(filter)
		{
			case MinNearestMagLinearMipNearest:
			case MinNearestMagMipLinear:
			case MinMagLinearMipNearest:
			case MinMagMipLinear: return VK_FILTER_LINEAR;
		}
		return VK_FILTER_NEAREST;
	}

	VkFilter convert_min_filter(Filter filter)
	{
		using enum Filter;
		switch(filter)
		{
			case MinLinearMagMipNearest:
			case MinLinearMagNearestMipLinear:
			case MinMagLinearMipNearest:
			case MinMagMipLinear: return VK_FILTER_LINEAR;
		}
		return VK_FILTER_NEAREST;
	}

	VkSamplerMipmapMode convert_mipmap_mode(Filter filter)
	{
		using enum Filter;
		switch(filter)
		{
			case MinMagNearestMipLinear:
			case MinNearestMagMipLinear:
			case MinLinearMagNearestMipLinear:
			case MinMagMipLinear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		}
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}

	VkSamplerAddressMode convert_address_mode(AddressMode mode)
	{
		using enum AddressMode;
		switch(mode)
		{ // clang-format off
			case Repeat:		 return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case ClampToEdge:	 return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case ClampToBorder:	 return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		}
		VT_UNREACHABLE();
	}

	export VkCompareOp convert_compare_op(CompareOp op)
	{
		using enum CompareOp;
		switch(op)
		{
			case Never:			 return VK_COMPARE_OP_NEVER;
			case Less:			 return VK_COMPARE_OP_LESS;
			case Equal:			 return VK_COMPARE_OP_EQUAL;
			case LessOrEqual:	 return VK_COMPARE_OP_LESS_OR_EQUAL;
			case Greater:		 return VK_COMPARE_OP_GREATER;
			case NotEqual:		 return VK_COMPARE_OP_NOT_EQUAL;
			case GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case Always:		 return VK_COMPARE_OP_ALWAYS;
		} // clang-format on
		VT_UNREACHABLE();
	}

	VkBorderColor convert_border_color(BorderColor color)
	{
		using enum BorderColor;
		switch(color)
		{
			case Transparent: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			case OpaqueBlack: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
			case OpaqueWhite: return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		}
		VT_UNREACHABLE();
	}

	export class VulkanSampler
	{
	public:
		VulkanSampler(DeviceApiTable const& api, SamplerSpecification const& spec)
		{
			VkSamplerCreateInfo const sampler_info {
				.sType					 = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter				 = convert_mag_filter(spec.filter),
				.minFilter				 = convert_min_filter(spec.filter),
				.mipmapMode				 = convert_mipmap_mode(spec.filter),
				.addressModeU			 = convert_address_mode(spec.u_address_mode),
				.addressModeV			 = convert_address_mode(spec.v_address_mode),
				.addressModeW			 = convert_address_mode(spec.w_address_mode),
				.mipLodBias				 = spec.mip_lod_bias,
				.anisotropyEnable		 = spec.filter == Filter::Anisotropic,
				.maxAnisotropy			 = static_cast<float>(spec.max_anisotropy),
				.compareEnable			 = spec.enable_compare,
				.compareOp				 = convert_compare_op(spec.compare_op),
				.minLod					 = spec.min_lod,
				.maxLod					 = spec.max_lod,
				.borderColor			 = convert_border_color(spec.border_color),
				.unnormalizedCoordinates = false,
			};
			VkSampler unowned_sampler;

			auto result = api.vkCreateSampler(api.device, &sampler_info, nullptr, &unowned_sampler);
			sampler.reset(unowned_sampler);
			VT_ASSERT_RESULT(result, "Failed to create Vulkan sampler.");
		}

		VkSampler ptr() const
		{
			return sampler.get();
		}

	private:
		UniqueVkSampler sampler;
	};
}
