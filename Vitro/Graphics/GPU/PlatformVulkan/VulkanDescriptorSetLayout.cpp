module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <vector>
export module vt.Graphics.Vulkan.DescriptorSetLayout;

import vt.Core.Array;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	VkDescriptorType convert_descriptor_type(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{ // clang-format off
			case DynamicSampler:	  return VK_DESCRIPTOR_TYPE_SAMPLER;
			case Texture:			  return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case RwTexture:			  return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case Buffer:			  return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			case RwBuffer:			  return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			case UniformBuffer:		  return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case StructuredBuffer:
			case RwStructuredBuffer:
			case ByteAddressBuffer:
			case RwByteAddressBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		}
		VT_UNREACHABLE();
	}

	export VkShaderStageFlags convert_shader_stage(ShaderStage stage)
	{
		using enum ShaderStage;
		switch(stage)
		{
			case All:		   return VK_SHADER_STAGE_ALL;
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
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			bindings.reserve(spec.bindings.size());

			auto stage = convert_shader_stage(spec.visibility);
			for(auto& binding : spec.bindings)
				bindings.emplace_back(VkDescriptorSetLayoutBinding {
					.binding			= binding.shader_register,
					.descriptorType		= convert_descriptor_type(binding.type),
					.descriptorCount	= binding.count,
					.stageFlags			= stage,
					.pImmutableSamplers = nullptr,
				});

			VkDescriptorSetLayoutCreateInfo const layout_info {
				.sType		  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.bindingCount = count(bindings),
				.pBindings	  = bindings.data(),
			};
			VkDescriptorSetLayout unowned_layout;

			auto result = api.vkCreateDescriptorSetLayout(api.device, &layout_info, nullptr, &unowned_layout);
			layout.reset(unowned_layout, api);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan descriptor set layout.");
		}

		VkDescriptorSetLayout ptr() const
		{
			return layout.get();
		}

	private:
		UniqueVkDescriptorSetLayout layout;
	};
}
