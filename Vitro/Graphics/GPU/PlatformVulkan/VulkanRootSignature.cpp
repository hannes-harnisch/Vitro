module;
#include "VitroCore/Macros.hpp"
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
			render_push_constant_stages(derive_render_push_constant_stages(spec))
		{
			SmallList<VkDescriptorSetLayout> layout_handles(spec.layouts.size());

			unsigned index	   = 0;
			auto	 handle_it = layout_handles.begin();
			for(auto& layout : spec.layouts)
			{
				auto handle	 = layout.vulkan.get_handle();
				*handle_it++ = handle;
				layout_indices.try_emplace(handle, index++);
			}

			// We're creating two pipeline layouts because this is the only way to allow specifying ShaderStage::All as a
			// push constant range visibility for render push constants without overwriting push constants set to compute.
			initialize_layout(render_pipeline_layout, render_push_constant_stages, spec, api, layout_handles);
			initialize_layout(compute_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, spec, api, layout_handles);
		}

		unsigned get_layout_index(VkDescriptorSetLayout layout) const
		{
			return layout_indices.find(layout)->second;
		}

		VkShaderStageFlags get_render_push_constant_stages() const
		{
			return render_push_constant_stages;
		}

		VkPipelineLayout get_render_layout_handle() const
		{
			return render_pipeline_layout.get();
		}

		VkPipelineLayout get_compute_layout_handle() const
		{
			return compute_pipeline_layout.get();
		}

	private:
		UniqueVkPipelineLayout								render_pipeline_layout;
		UniqueVkPipelineLayout								compute_pipeline_layout;
		std::unordered_map<VkDescriptorSetLayout, unsigned> layout_indices;
		VkShaderStageFlags									render_push_constant_stages;

		static VkShaderStageFlags derive_render_push_constant_stages(RootSignatureSpecification const& spec)
		{
			auto stage = SHADER_STAGE_LOOKUP[spec.push_constants_visibility];

			// If all shader stages were specified, we want to remove the compute stage.
			return stage ^ VK_SHADER_STAGE_COMPUTE_BIT;
		}

		static void initialize_layout(UniqueVkPipelineLayout&			pipeline_layout,
									  VkShaderStageFlags				stages,
									  RootSignatureSpecification const& spec,
									  DeviceApiTable const&				api,
									  ConstSpan<VkDescriptorSetLayout>	layouts)
		{
			VkPushConstantRange const push_constants {
				.stageFlags = stages,
				.offset		= 0,
				.size		= spec.push_constants_byte_size,
			};
			VkPipelineLayoutCreateInfo const layout_info {
				.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				.setLayoutCount			= count(layouts),
				.pSetLayouts			= layouts.data(),
				.pushConstantRangeCount = 1,
				.pPushConstantRanges	= &push_constants,
			};
			auto result = api.vkCreatePipelineLayout(api.device, &layout_info, nullptr, std::out_ptr(pipeline_layout, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan pipeline layout.");
		}
	};
}
