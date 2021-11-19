module;
#include "VitroCore/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
export module vt.Graphics.Vulkan.Sampler;

import vt.Core.LookupTable;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.Vulkan.Handle;

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

	constexpr inline auto ADDRESS_MODE_LOOKUP = [] {
		LookupTable<AddressMode, VkSamplerAddressMode> _;
		using enum AddressMode;

		_[Repeat]		  = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		_[MirroredRepeat] = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		_[ClampToEdge]	  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		_[ClampToBorder]  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		return _;
	}();

	export constexpr inline auto COMPARE_OP_LOOKUP = [] {
		LookupTable<CompareOp, VkCompareOp> _;
		using enum CompareOp;

		_[Never]		  = VK_COMPARE_OP_NEVER;
		_[Less]			  = VK_COMPARE_OP_LESS;
		_[Equal]		  = VK_COMPARE_OP_EQUAL;
		_[LessOrEqual]	  = VK_COMPARE_OP_LESS_OR_EQUAL;
		_[Greater]		  = VK_COMPARE_OP_GREATER;
		_[NotEqual]		  = VK_COMPARE_OP_NOT_EQUAL;
		_[GreaterOrEqual] = VK_COMPARE_OP_GREATER_OR_EQUAL;
		_[Always]		  = VK_COMPARE_OP_ALWAYS;
		return _;
	}();

	constexpr inline auto BORDER_COLOR_LOOKUP = [] {
		LookupTable<BorderColor, VkBorderColor> _;
		using enum BorderColor;

		_[Transparent] = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		_[OpaqueBlack] = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		_[OpaqueWhite] = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		return _;
	}();

	export class VulkanSampler
	{
	public:
		VulkanSampler(SamplerSpecification const& spec, DeviceApiTable const& api)
		{
			VkSamplerCreateInfo const sampler_info {
				.sType					 = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter				 = convert_mag_filter(spec.filter),
				.minFilter				 = convert_min_filter(spec.filter),
				.mipmapMode				 = convert_mipmap_mode(spec.filter),
				.addressModeU			 = ADDRESS_MODE_LOOKUP[spec.u_address_mode],
				.addressModeV			 = ADDRESS_MODE_LOOKUP[spec.v_address_mode],
				.addressModeW			 = ADDRESS_MODE_LOOKUP[spec.w_address_mode],
				.mipLodBias				 = spec.mip_lod_bias,
				.anisotropyEnable		 = spec.filter == Filter::Anisotropic,
				.maxAnisotropy			 = static_cast<float>(spec.max_anisotropy),
				.compareEnable			 = spec.enable_compare,
				.compareOp				 = COMPARE_OP_LOOKUP[spec.compare_op],
				.minLod					 = spec.min_lod,
				.maxLod					 = spec.max_lod,
				.borderColor			 = BORDER_COLOR_LOOKUP[spec.border_color],
				.unnormalizedCoordinates = false,
			};
			auto result = api.vkCreateSampler(api.device, &sampler_info, nullptr, std::out_ptr(sampler, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan sampler.");
		}

		VkSampler get_handle() const
		{
			return sampler.get();
		}

	private:
		UniqueVkSampler sampler;
	};
}
