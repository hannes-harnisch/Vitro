module;
#include "VitroCore/Macros.hpp"
#include "VulkanAPI.hpp"

#include <algorithm>
#include <memory>
#include <ranges>
export module vt.Graphics.Vulkan.DescriptorPool;

import vt.Core.Array;
import vt.Core.LookupTable;
import vt.Core.SmallList;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Vulkan.Handle;
import vt.Trace.Log;

namespace vt::vulkan
{
	constexpr inline auto MAX_SIZES = [] {
		LookupTable<VkDescriptorType, uint32_t, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> _;

		// Feel free to change any of these should the need arise.
		_[VK_DESCRIPTOR_TYPE_SAMPLER]			   = 256;
		_[VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE]		   = 10000;
		_[VK_DESCRIPTOR_TYPE_STORAGE_IMAGE]		   = 128;
		_[VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER] = 64;
		_[VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER] = 64;
		_[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]	   = 128;
		_[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER]	   = 128;
		_[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT]	   = 128;
		return _;
	}();

	VkDescriptorPoolSize make_pool_size(VkDescriptorType type, uint32_t limit)
	{
		return {
			.type			 = type,
			.descriptorCount = std::min(MAX_SIZES[type], limit),
		};
	}

	export class DescriptorPool
	{
	public:
		static constexpr uint32_t MAX_DESCRIPTOR_SETS = 8192;

		DescriptorPool(DeviceApiTable const& api, VkPhysicalDeviceProperties const& properties)
		{
			auto& lim = properties.limits;

			VkDescriptorPoolSize sizes[] {
				make_pool_size(VK_DESCRIPTOR_TYPE_SAMPLER, lim.maxPerStageDescriptorSamplers),
				make_pool_size(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, lim.maxPerStageDescriptorSampledImages),
				make_pool_size(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, lim.maxPerStageDescriptorStorageImages),
				{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, MAX_SIZES[VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER]},
				{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, MAX_SIZES[VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER]},
				make_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, lim.maxPerStageDescriptorUniformBuffers),
				make_pool_size(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, lim.maxPerStageDescriptorStorageBuffers),
				make_pool_size(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, lim.maxPerStageDescriptorInputAttachments),
			};
			max_sampler_descriptors	 = sizes[0].descriptorCount;
			max_resource_descriptors = count_max_resource_descriptors(sizes);

			VkDescriptorPoolCreateInfo const pool_info {
				.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.maxSets	   = MAX_DESCRIPTOR_SETS,
				.poolSizeCount = count(sizes),
				.pPoolSizes	   = sizes,
			};
			auto result = api.vkCreateDescriptorPool(api.device, &pool_info, nullptr, std::out_ptr(pool, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan descriptor pool.");
		}

		SmallList<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> layouts,
													  unsigned const				 dynamic_counts[],
													  DeviceApiTable const&			 api)
		{
			SmallList<VkDescriptorSetLayout> set_layouts;
			set_layouts.reserve(layouts.size());
			for(auto& layout : layouts)
				set_layouts.emplace_back(layout.vulkan.get_handle());

			SmallList<VkDescriptorSet> sets(layouts.size());

			VkDescriptorSetVariableDescriptorCountAllocateInfo const variable_count_info {
				.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
				.descriptorSetCount = count(set_layouts),
				.pDescriptorCounts	= dynamic_counts,
			};
			VkDescriptorSetAllocateInfo const allocate_info {
				.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.pNext				= &variable_count_info,
				.descriptorPool		= pool.get(),
				.descriptorSetCount = variable_count_info.descriptorSetCount,
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

		unsigned get_max_resource_descriptors() const
		{
			return max_resource_descriptors;
		}

		unsigned get_max_sampler_descriptors() const
		{
			return max_sampler_descriptors;
		}

		VkDescriptorPool get_handle() const
		{
			return pool.get();
		}

	private:
		UniqueVkDescriptorPool pool;
		unsigned			   max_sampler_descriptors;
		unsigned			   max_resource_descriptors;

		static unsigned count_max_resource_descriptors(ConstSpan<VkDescriptorPoolSize> sizes)
		{
			unsigned count = 0;

			for(auto size : sizes | std::views::drop(1))
				count += size.descriptorCount;

			return count;
		}
	};
}
