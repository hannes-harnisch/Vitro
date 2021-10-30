module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
export module vt.Graphics.Vulkan.RootSignature;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Vulkan.DescriptorSetLayout;
import vt.Graphics.Vulkan.Handle;

namespace vt::vulkan
{
	export class VulkanRootSignature
	{
	public:
		VulkanRootSignature(RootSignatureSpecification const& spec, DeviceApiTable const& api) :
			push_constant_stages(convert_shader_stage(spec.push_constants_visibility))
		{
			SmallList<VkDescriptorSetLayout> set_layouts;
			set_layouts.reserve(spec.layouts.size());

			unsigned index = 0;
			for(auto& layout : spec.layouts)
			{
				auto handle = layout.vulkan.get_handle();
				set_layouts.emplace_back(handle);
				layout_indices.try_emplace(handle, index++);
			}

			VkPushConstantRange const push_constants {
				.stageFlags = push_constant_stages,
				.offset		= 0,
				.size		= spec.push_constants_byte_size,
			};
			VkPipelineLayoutCreateInfo const layout_info {
				.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				.setLayoutCount			= count(set_layouts),
				.pSetLayouts			= set_layouts.data(),
				.pushConstantRangeCount = 1,
				.pPushConstantRanges	= &push_constants,
			};
			auto result = api.vkCreatePipelineLayout(api.device, &layout_info, nullptr, std::out_ptr(pipeline_layout, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan pipeline layout.");
		}

		unsigned get_layout_index(VkDescriptorSetLayout layout) const
		{
			return layout_indices.find(layout)->second;
		}

		VkShaderStageFlags get_push_constant_stages() const
		{
			return push_constant_stages;
		}

		VkPipelineLayout get_handle() const
		{
			return pipeline_layout.get();
		}

	private:
		UniqueVkPipelineLayout								pipeline_layout;
		std::unordered_map<VkDescriptorSetLayout, unsigned> layout_indices;
		VkShaderStageFlags									push_constant_stages;
	};
}
