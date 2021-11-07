module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <vector>
export module vt.Graphics.Vulkan.DescriptorSetLayout;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.Vulkan.Handle;
import vt.Graphics.Vulkan.Sampler;

namespace vt::vulkan
{
	VkDescriptorType convert_descriptor_type(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{ // clang-format off
			case Sampler:			  return VK_DESCRIPTOR_TYPE_SAMPLER;
			case Texture:			  return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case RwTexture:			  return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case Buffer:			  return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			case RwBuffer:			  return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			case UniformBuffer:		  return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case StructuredBuffer:
			case RwStructuredBuffer:
			case ByteAddressBuffer:
			case RwByteAddressBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case InputAttachment:	  return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		}
		VT_UNREACHABLE();
	}

	export VkShaderStageFlags convert_shader_stage(ShaderStage stage)
	{
		using enum ShaderStage;
		switch(stage)
		{
			case Render:	   return VK_SHADER_STAGE_ALL_GRAPHICS;
			case Vertex:	   return VK_SHADER_STAGE_VERTEX_BIT;
			case Hull:		   return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case Domain:	   return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case Fragment:	   return VK_SHADER_STAGE_FRAGMENT_BIT;
			case Compute:	   return VK_SHADER_STAGE_COMPUTE_BIT;
			case RayGen:	   return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			case AnyHit:	   return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
			case ClosestHit:   return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			case Miss:		   return VK_SHADER_STAGE_MISS_BIT_KHR;
			case Intersection: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
			case Callable:	   return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		} // clang-format on
		VT_UNREACHABLE();
	}

	export class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout(DescriptorSetLayoutSpecification const& spec, DeviceApiTable const& api)
		{
			SmallList<VkDescriptorSetLayoutBinding> bindings;
			bindings.reserve(spec.bindings.size());

			auto visibility = convert_shader_stage(spec.visibility);
			for(auto& binding : spec.bindings)
			{
				VkSampler static_sampler;
				if(binding.static_sampler_spec)
					static_sampler = static_samplers.emplace_back(*binding.static_sampler_spec, api).get_handle();

				bindings.emplace_back(VkDescriptorSetLayoutBinding {
					.binding			= binding.shader_register,
					.descriptorType		= convert_descriptor_type(binding.type),
					.descriptorCount	= binding.static_sampler_spec ? 1u : binding.count.get(),
					.stageFlags			= binding.static_sampler_spec ? convert_shader_stage(binding.static_sampler_visibility)
																	  : visibility,
					.pImmutableSamplers = binding.static_sampler_spec ? &static_sampler : nullptr,
				});
			}

			VkDescriptorSetLayoutCreateInfo const layout_info {
				.sType		  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.bindingCount = count(bindings),
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
