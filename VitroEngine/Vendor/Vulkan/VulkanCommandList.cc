module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.CommandList;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Graphics.CommandListBase;
import vt.Graphics.Device;
import vt.Graphics.Handle;
import vt.Graphics.Resource;
import vt.Graphics.RootSignature;
import vt.Vulkan.DeviceChild;
import vt.Vulkan.Driver;

namespace vt::vulkan
{
	template<CommandType Type> class CommandListData;

	template<> class CommandListData<CommandType::Copy>
	{};

	template<> class CommandListData<CommandType::Compute> : protected CommandListData<CommandType::Copy>
	{
	protected:
		VkPipelineLayout current_compute_layout = nullptr;
	};

	template<> class CommandListData<CommandType::Render> : protected CommandListData<CommandType::Compute>
	{
	protected:
		VkPipelineLayout current_render_layout = nullptr;
	};

	export template<CommandType Type>
	class VulkanCommandList final : public CommandListBase<Type>, private CommandListData<Type>, private DeviceChild
	{
	public:
		VulkanCommandList(Device const& device) : DeviceChild(device)
		{
			VkCommandPoolCreateInfo const pool_info {
				.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.queueFamilyIndex = 0,
			};
			VkCommandPool raw_pool;

			auto result = api->vkCreateCommandPool(api->device, &pool_info, nullptr, &raw_pool);
			VT_ASSERT_RESULT(result, "Failed to create Vulkan command pool.");
			pool.reset(raw_pool);

			VkCommandBufferAllocateInfo const alloc_info {
				.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool		= pool.get(),
				.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};
			result = api->vkAllocateCommandBuffers(api->device, &alloc_info, &cmd);
			VT_ASSERT_RESULT(result, "Failed to allocate Vulkan command buffer.");
		}

		CommandListHandle handle()
		{
			return {cmd};
		}

		void reset()
		{
			auto result = api->vkResetCommandPool(api->device, pool.get(), 0);
			VT_ASSERT_RESULT(result, "Failed to reset Vulkan command pool.");
		}

		void begin()
		{
			VkCommandBufferBeginInfo const begin_info {
				.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.flags			  = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
				.pInheritanceInfo = nullptr,
			};
			auto result = api->vkBeginCommandBuffer(cmd, &begin_info);
			VT_ASSERT_RESULT(result, "Failed to begin Vulkan command buffer.");
		}

		void end()
		{
			auto result = api->vkEndCommandBuffer(cmd);
			VT_ASSERT_RESULT(result, "Failed to end Vulkan command buffer.");
		}

		void bind_descriptor_pool(DescriptorPool const&)
		{
			// Do nothing, as Vulkan covers binding descriptor pools when binding descriptor sets.
		}

		void bind_compute_pipeline(Pipeline const& pipeline)
		{
			api->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.vulkan.ptr());
		}

		void bind_compute_root_signature(RootSignature const& root_signature)
		{
			this->current_compute_layout = root_signature.vulkan.ptr();
		}

		void bind_compute_descriptors(CSpan<DescriptorSet> descriptor_sets)
		{
			api->vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, this->current_compute_layout, 0,
										 count(descriptor_sets), descriptor_sets.data(), 0, nullptr);
		}

		void push_compute_constants(unsigned size_in_32bit_units, void const* data, unsigned offset_in_32bit_units)
		{
			unsigned offset = offset_in_32bit_units / 4;
			unsigned size	= size_in_32bit_units / 4;
			api->vkCmdPushConstants(cmd, this->current_compute_layout, VK_SHADER_STAGE_COMPUTE_BIT, offset, size, data);
		}

		void dispatch(unsigned x_count, unsigned y_count, unsigned z_count)
		{
			api->vkCmdDispatch(cmd, x_count, y_count, z_count);
		}

		void bind_render_pipeline(Pipeline const& pipeline)
		{
			api->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vulkan.ptr());
		}

		void bind_render_root_signature(RootSignature const& root_signature)
		{
			this->current_render_layout = root_signature.vulkan.ptr();
		}

		void bind_render_descriptors(CSpan<DescriptorSet> descriptor_sets)
		{
			api->vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, this->current_render_layout, 0,
										 count(descriptor_sets), descriptor_sets.data(), 0, nullptr);
		}

		void push_render_constants(unsigned size_in_32bit_units, void const* data, unsigned offset_in_32bit_units)
		{
			unsigned offset = offset_in_32bit_units / 4;
			unsigned size	= size_in_32bit_units / 4;
			api->vkCmdPushConstants(cmd, this->current_render_layout, VK_SHADER_STAGE_ALL_GRAPHICS, offset, size, data);
		}

	private:
		UniqueVkCommandPool pool;
		VkCommandBuffer		cmd;
	};
}
