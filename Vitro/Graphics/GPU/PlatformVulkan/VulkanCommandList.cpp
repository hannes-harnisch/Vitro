module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <ranges>
#include <type_traits>
#include <vector>
export module vt.Graphics.Vulkan.CommandList;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Rectangle;
import vt.Graphics.AssetResource;
import vt.Graphics.CommandListBase;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Handle;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;
import vt.Graphics.Vulkan.Driver;
import vt.Graphics.Vulkan.RootSignature;

namespace vt::vulkan
{
	template<CommandType> class CommandListData;

	template<> class CommandListData<CommandType::Copy>
	{};

	template<> class CommandListData<CommandType::Compute> : protected CommandListData<CommandType::Copy>
	{
	protected:
		VulkanRootSignature const* bound_compute_layout = nullptr;
	};

	template<> class CommandListData<CommandType::Render> : protected CommandListData<CommandType::Compute>
	{
	protected:
		VulkanRootSignature const* bound_render_layout = nullptr;
	};

	export template<CommandType TYPE>
	class VulkanCommandList final : public CommandListBase<TYPE>, private CommandListData<TYPE>
	{
	public:
		VulkanCommandList(unsigned queue_family, DeviceApiTable const& api) : api(&api)
		{
			VkCommandPoolCreateInfo const pool_info {
				.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.queueFamilyIndex = queue_family,
			};
			VkCommandPool unowned_pool;

			auto result = api.vkCreateCommandPool(api.device, &pool_info, nullptr, &unowned_pool);
			pool.reset(unowned_pool, api);
			VT_ASSERT_RESULT(result, "Failed to create Vulkan command pool.");

			VkCommandBufferAllocateInfo const alloc_info {
				.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool		= pool.get(),
				.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};
			result = api.vkAllocateCommandBuffers(api.device, &alloc_info, &cmd);
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

		void bind_compute_pipeline(ComputePipeline const& pipeline)
		{
			api->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.vulkan.ptr());
		}

		void bind_compute_root_signature(RootSignature const& root_signature)
		{
			this->bound_compute_layout = &root_signature.vulkan;
		}

		void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			bind_descriptor_sets(VK_PIPELINE_BIND_POINT_COMPUTE, *this->bound_compute_layout, descriptor_sets);
		}

		void push_compute_constants(size_t byte_offset, size_t byte_size, void const* data)
		{
			api->vkCmdPushConstants(cmd, this->bound_compute_layout->ptr(), VK_SHADER_STAGE_COMPUTE_BIT,
									static_cast<unsigned>(byte_offset), static_cast<unsigned>(byte_size), data);
		}

		void dispatch(unsigned x_count, unsigned y_count, unsigned z_count)
		{
			api->vkCmdDispatch(cmd, x_count, y_count, z_count);
		}

		void begin_render_pass(RenderPass const&	 render_pass,
							   RenderTarget const&	 render_target,
							   ConstSpan<ClearValue> clear_values = {})
		{
			VkRenderPassBeginInfo const begin_info {
				.sType		 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				.renderPass	 = render_pass.vulkan.ptr(),
				.framebuffer = render_target.vulkan.get_framebuffer(),
				.renderArea {
					.offset {
						.x = 0,
						.y = 0,
					},
					.extent {
						.width	= render_target.get_width(),
						.height = render_target.get_height(),
					},
				},
				.clearValueCount = count(clear_values),
				.pClearValues	 = reinterpret_cast<VkClearValue const*>(clear_values.data()),
			};
			api->vkCmdBeginRenderPass(cmd, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
		}

		void transition_subpass()
		{
			api->vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);
		}

		void end_render_pass()
		{
			api->vkCmdEndRenderPass(cmd);
		}

		void bind_render_pipeline(RenderPipeline const& pipeline)
		{
			api->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vulkan.ptr());
		}

		void bind_render_root_signature(RootSignature const& root_signature)
		{
			this->bound_render_layout = &root_signature.vulkan;
		}

		void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, *this->bound_render_layout, descriptor_sets);
		}

		void push_render_constants(size_t byte_offset, size_t byte_size, void const* data)
		{
			api->vkCmdPushConstants(cmd, this->bound_render_layout->ptr(), VK_SHADER_STAGE_ALL_GRAPHICS,
									static_cast<unsigned>(byte_offset), static_cast<unsigned>(byte_size), data);
		}

		void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets)
		{
			FixedList<VkBuffer, MAX_VERTEX_BUFFERS> handles(first_buffer);
			for(auto& buffer : std::views::take(buffers, first_buffer))
				handles.emplace_back(buffer.vulkan.ptr());

			api->vkCmdBindVertexBuffers(cmd, first_buffer, count(handles), handles.data(), byte_offsets.data());
		}

		void bind_index_buffer(Buffer const& buffer, size_t byte_offset)
		{
			auto index_type = get_index_type_from_stride(buffer.get_stride());
			api->vkCmdBindIndexBuffer(cmd, buffer.vulkan.ptr(), byte_offset, index_type);
		}

		void set_viewports(ArrayView<Viewport> viewports)
		{
			static_assert(std::is_layout_compatible_v<Viewport, VkViewport>);

			auto data = reinterpret_cast<VkViewport const*>(viewports.data());
			api->vkCmdSetViewport(cmd, 0, count(viewports), data);
		}

		void set_scissors(ArrayView<Rectangle> scissors)
		{
			// static_assert(std::is_layout_compatible_v<Rectangle, VkRect2D>); // TODO: await type trait fix in MSVC

			auto data = reinterpret_cast<VkRect2D const*>(scissors.data());
			api->vkCmdSetScissor(cmd, 0, count(scissors), data);
		}

		void set_blend_constants(Float4 blend_constants)
		{
			float const constants[] {
				blend_constants.r,
				blend_constants.g,
				blend_constants.b,
				blend_constants.a,
			};
			api->vkCmdSetBlendConstants(cmd, constants);
		}

		void set_depth_bounds(float min, float max)
		{
			api->vkCmdSetDepthBounds(cmd, min, max);
		}

		void set_stencil_reference(unsigned reference_value)
		{
			api->vkCmdSetStencilReference(cmd, VK_STENCIL_FACE_FRONT_AND_BACK, reference_value);
		}

		void draw(unsigned vertex_count, unsigned instance_count, unsigned first_vertex, unsigned first_instance)
		{
			api->vkCmdDraw(cmd, vertex_count, instance_count, first_vertex, first_instance);
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
		DeviceApiTable const* api;
		UniqueVkCommandPool	  pool;
		VkCommandBuffer		  cmd;

		static VkIndexType get_index_type_from_stride(unsigned stride)
		{
			switch(stride)
			{
				case 2: return VK_INDEX_TYPE_UINT16;
				case 4: return VK_INDEX_TYPE_UINT32;
			}
			VT_UNREACHABLE();
		}

		void bind_descriptor_sets(VkPipelineBindPoint		 bind_point,
								  VulkanRootSignature const& root_signature,
								  ArrayView<DescriptorSet>	 descriptor_sets) const
		{
			std::vector<VkDescriptorSet> sets;
			sets.reserve(descriptor_sets.size());
			auto layout = root_signature.ptr();

			sets.emplace_back(descriptor_sets.front().vulkan.ptr());
			unsigned start_index = root_signature.get_layout_index(descriptor_sets.front().vulkan.get_layout());
			unsigned prev_index	 = start_index;

			for(auto& set : descriptor_sets | std::views::drop(1))
			{
				unsigned index = root_signature.get_layout_index(set.vulkan.get_layout());
				if(index != prev_index + 1)
				{
					api->vkCmdBindDescriptorSets(cmd, bind_point, layout, start_index, count(sets), sets.data(), 0, nullptr);
					sets.clear();
					start_index = index;
				}
				sets.emplace_back(set.vulkan.ptr());
				++prev_index;
			}
			api->vkCmdBindDescriptorSets(cmd, bind_point, layout, start_index, count(sets), sets.data(), 0, nullptr);
		}
	};
}
