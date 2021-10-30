module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <vector>
export module vt.Graphics.Vulkan.DescriptorPool;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Vulkan.Handle;
import vt.Trace.Log;

namespace vt::vulkan
{
	export class DescriptorPool
	{
	public:
		DescriptorPool(DeviceApiTable const& api)
		{
			VkDescriptorPoolCreateInfo const pool_info {
				.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.maxSets	   = MAX_DESCRIPTOR_SETS,
				.poolSizeCount = count(POOL_SIZES),
				.pPoolSizes	   = POOL_SIZES,
			};
			auto result = api.vkCreateDescriptorPool(api.device, &pool_info, nullptr, std::out_ptr(pool, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan descriptor pool.");
		}

		SmallList<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> layouts, DeviceApiTable const& api)
		{
			SmallList<VkDescriptorSetLayout> set_layouts;
			set_layouts.reserve(layouts.size());
			for(auto& layout : layouts)
				set_layouts.emplace_back(layout.vulkan.get_handle());

			SmallList<VkDescriptorSet> sets(layouts.size());

			VkDescriptorSetAllocateInfo const allocate_info {
				.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.descriptorPool		= pool.get(),
				.descriptorSetCount = count(set_layouts),
				.pSetLayouts		= set_layouts.data(),
			};
			auto result = api.vkAllocateDescriptorSets(api.device, &allocate_info, sets.data());
			VT_CHECK_RESULT(result, "Failed to allocate Vulkan descriptor sets.");

			SmallList<DescriptorSet> final_sets;
			final_sets.reserve(layouts.size());
			auto layout = set_layouts.begin();
			for(auto set : sets)
				final_sets.emplace_back(VulkanDescriptorSet(set, *layout++));

			return final_sets;
		}

	private:
		// Feel free to change any of these if necessary.
		static constexpr VkDescriptorPoolSize POOL_SIZES[] {
			{VK_DESCRIPTOR_TYPE_SAMPLER, 256},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 10000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 256},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 256},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 256},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 256},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 256},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 256},
		};
		static constexpr uint32_t MAX_DESCRIPTOR_SETS = 8192;

		UniqueVkDescriptorPool pool;
	};
}
