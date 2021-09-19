module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"

#include <ranges>
#include <type_traits>
export module vt.Vulkan.CommandList;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Rectangle;
import vt.Graphics.CommandListBase;
import vt.Graphics.DescriptorPool;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Device;
import vt.Graphics.Handle;
import vt.Graphics.PipelineInfo;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.Resource;
import vt.Graphics.RootSignature;
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
	class VulkanCommandList final : public CommandListBase<Type>, private CommandListData<Type>
	{
	public:
		VulkanCommandList(Device const& device) : api(device.vulkan.get_api())
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

		void push_compute_constants(unsigned offset_in_32bit_units, unsigned size_in_32bit_units, void const* data)
		{
			unsigned offset = offset_in_32bit_units * BytesPerPushConstantsUnit;
			unsigned size	= size_in_32bit_units * BytesPerPushConstantsUnit;
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

		void push_render_constants(unsigned offset_in_32bit_units, unsigned size_in_32bit_units, void const* data)
		{
			unsigned offset = offset_in_32bit_units * BytesPerPushConstantsUnit;
			unsigned size	= size_in_32bit_units * BytesPerPushConstantsUnit;
			api->vkCmdPushConstants(cmd, this->current_render_layout, VK_SHADER_STAGE_ALL_GRAPHICS, offset, size, data);
		}

		void begin_render_pass(RenderPass const&   render_pass,
							   RenderTarget const& render_target,
							   CSpan<ClearValue>   clear_values = {})
		{
			VkRenderPassBeginInfo const begin_info {
				.sType		 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				.renderPass	 = render_pass.vulkan.ptr(),
				.framebuffer = render_target.vulkan.get_frame_buffer(),
				.renderArea {
					.offset {
						.x = 0,
						.y = 0,
					},
					.extent {
						.width	= render_target.vulkan.get_width(),
						.height = render_target.vulkan.get_height(),
					},
				},
				.clearValueCount = count(clear_values),
				.pClearValues	 = reinterpret_cast<VkClearValue const*>(clear_values.data()),
			};
			api->vkCmdBeginRenderPass(cmd, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
		}

		void transition_to_next_subpass()
		{
			api->vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);
		}

		void end_render_pass()
		{
			api->vkCmdEndRenderPass(cmd);
		}

		void bind_vertex_buffers(unsigned first_buffer, CSpan<Buffer> buffers, CSpan<size_t> byte_offsets)
		{
			FixedList<VkBuffer, MaxVertexAttributes> handles(first_buffer);
			for(auto& buffer : std::views::take(buffers, first_buffer))
				handles.emplace_back(buffer.vulkan.ptr());

			api->vkCmdBindVertexBuffers(cmd, first_buffer, count(handles), handles.data(), byte_offsets.data());
		}

		void bind_index_buffer(Buffer const& buffer, size_t byte_offset)
		{
			auto index_type = get_index_type_from_stride(buffer.get_stride());
			api->vkCmdBindIndexBuffer(cmd, buffer.vulkan.ptr(), byte_offset, index_type);
		}

		void bind_primitive_topology(PrimitiveTopology)
		{
			// Do nothing, as Vulkan covers binding the primitive topology when binding a pipeline.
		}

		void bind_viewports(CSpan<Viewport> viewports)
		{
			static_assert(std::is_layout_compatible_v<Viewport, VkViewport>);
			auto data = reinterpret_cast<VkViewport const*>(viewports.data());

			api->vkCmdSetViewport(cmd, 0, count(viewports), data);
		}

		void bind_scissors(CSpan<Rectangle> scissors)
		{
			static_assert(std::is_layout_compatible_v<Rectangle, VkRect2D>);
			auto data = reinterpret_cast<VkRect2D const*>(scissors.data());

			api->vkCmdSetScissor(cmd, 0, count(scissors), data);
		}

		void draw(unsigned vertex_count, unsigned instance_count, unsigned first_vertex, unsigned first_instance)
		{
			api->vkCmdDraw(cmd, vertex_count, instance_count, first_vertex, first_buffer);
		}

		void draw_indexed(unsigned index_count,
						  unsigned instance_count,
						  unsigned first_index,
						  int	   vertex_offset,
						  unsigned first_instance)
		{
			api->vkCmdDrawIndexed(cmd, index_count, instance_count, first_index, vertex_offset, first_instance);
		}

	private:
		DeviceFunctionTable const* api;
		UniqueVkCommandPool		   pool;
		VkCommandBuffer			   cmd;

		static VkIndexType get_index_type_from_stride(unsigned stride)
		{
			switch(stride)
			{
				case 2: return VK_INDEX_TYPE_UINT16;
				case 4: return VK_INDEX_TYPE_UINT32;
			}
			VT_UNREACHABLE();
		}
	};
}
