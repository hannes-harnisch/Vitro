module;
#include "VitroCore/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <vector>
export module vt.Graphics.Vulkan.DescriptorSetLayout;

import vt.Core.Array;
import vt.Core.LookupTable;
import vt.Core.SmallList;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.Vulkan.Handle;
import vt.Graphics.Vulkan.Sampler;
import vttool.HlslBuilder.VulkanRegisterShiftOffsets;

namespace vt::vulkan
{
	export constexpr inline auto DESCRIPTOR_TYPE_LOOKUP = [] {
		LookupTable<DescriptorType, VkDescriptorType> _;
		using enum DescriptorType;

		_[Sampler]			   = VK_DESCRIPTOR_TYPE_SAMPLER;
		_[Texture]			   = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		_[RwTexture]		   = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		_[Buffer]			   = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		_[RwBuffer]			   = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		_[UniformBuffer]	   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		_[StructuredBuffer]	   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		_[RwStructuredBuffer]  = _[StructuredBuffer];
		_[ByteAddressBuffer]   = _[StructuredBuffer];
		_[RwByteAddressBuffer] = _[StructuredBuffer];
		_[InputAttachment]	   = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		return _;
	}();

	export constexpr inline auto SHADER_STAGE_LOOKUP = [] {
		LookupTable<ShaderStage, VkShaderStageFlags> _;
		using enum ShaderStage;

		_[All]			= VK_SHADER_STAGE_ALL;
		_[Vertex]		= VK_SHADER_STAGE_VERTEX_BIT;
		_[Hull]			= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		_[Domain]		= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		_[Fragment]		= VK_SHADER_STAGE_FRAGMENT_BIT;
		_[Compute]		= VK_SHADER_STAGE_COMPUTE_BIT;
		_[RayGen]		= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		_[AnyHit]		= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		_[ClosestHit]	= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		_[Miss]			= VK_SHADER_STAGE_MISS_BIT_KHR;
		_[Intersection] = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		_[Callable]		= VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		return _;
	}();

	export constexpr inline auto REGISTER_OFFSET_LOOKUP = [] {
		LookupTable<VkDescriptorType, unsigned, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> _;
		_[VK_DESCRIPTOR_TYPE_SAMPLER]			   = tool::S_BINDING_OFFSET;
		_[VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE]		   = tool::T_BINDING_OFFSET;
		_[VK_DESCRIPTOR_TYPE_STORAGE_IMAGE]		   = tool::U_BINDING_OFFSET;
		_[VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER] = tool::T_BINDING_OFFSET;
		_[VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER] = tool::U_BINDING_OFFSET;
		_[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]	   = tool::B_BINDING_OFFSET;
		_[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER]	   = tool::U_BINDING_OFFSET;
		_[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT]	   = tool::U_BINDING_OFFSET;
		return _;
	}();

	export class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout(DescriptorSetLayoutSpecification const& spec, DeviceApiTable const& api)
		{
			SmallList<VkDescriptorSetLayoutBinding> bindings;
			SmallList<VkDescriptorBindingFlags>		flags;
			bindings.reserve(spec.bindings.size());
			flags.reserve(spec.bindings.size());

			auto visibility = SHADER_STAGE_LOOKUP[spec.visibility];
			for(auto& binding : spec.bindings)
			{
				VkSampler static_sampler;
				if(binding.static_sampler_spec)
					static_sampler = static_samplers.emplace_back(*binding.static_sampler_spec, api).get_handle();

				auto type = DESCRIPTOR_TYPE_LOOKUP[binding.type];
				bindings.emplace_back(VkDescriptorSetLayoutBinding {
					.binding			= binding.shader_register + REGISTER_OFFSET_LOOKUP[type],
					.descriptorType		= type,
					.descriptorCount	= binding.static_sampler_spec ? 1u : binding.count.get(),
					.stageFlags			= binding.static_sampler_spec ? SHADER_STAGE_LOOKUP[binding.static_sampler_visibility]
																	  : visibility,
					.pImmutableSamplers = binding.static_sampler_spec ? &static_sampler : nullptr,
				});

				if(binding.count == UINT_MAX)
					flags.emplace_back(VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT);
				else
					flags.emplace_back(0);
			}

			VkDescriptorSetLayoutBindingFlagsCreateInfo const flags_info {
				.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
				.bindingCount  = count(bindings),
				.pBindingFlags = flags.data(),
			};
			VkDescriptorSetLayoutCreateInfo const layout_info {
				.sType		  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext		  = &flags_info,
				.bindingCount = flags_info.bindingCount,
				.pBindings	  = bindings.data(),
			};
			auto result = api.vkCreateDescriptorSetLayout(api.device, &layout_info, nullptr, std::out_ptr(layout, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan descriptor set layout.");
		}

		VkDescriptorSetLayout get_handle() const
		{
			return layout.get();
		}

	private:
		UniqueVkDescriptorSetLayout layout;
		std::vector<VulkanSampler>	static_samplers;
	};
}
