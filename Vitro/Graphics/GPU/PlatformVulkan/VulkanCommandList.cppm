module;
#include "VitroCore/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <ranges>
#include <type_traits>
#include <vector>
export module vt.Graphics.Vulkan.CommandList;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Rect;
import vt.Graphics.AssetResource;
import vt.Graphics.AbstractCommandList;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Handle;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;
import vt.Graphics.Vulkan.RootSignature;

namespace vt::vulkan
{
	template<CommandType> class CommandListData;

	template<> class CommandListData<CommandType::Copy>
	{};

	template<> class CommandListData<CommandType::Compute> : protected CommandListData<CommandType::Copy>
	{
	protected:
		CommandListPipelineLayoutData bound_compute_layout;
	};

	template<> class CommandListData<CommandType::Render> : protected CommandListData<CommandType::Compute>
	{
	protected:
		CommandListPipelineLayoutData bound_render_layout;
	};

	export template<CommandType TYPE>
	class VulkanCommandList final : public AbstractCommandList<TYPE>, private CommandListData<TYPE>
	{
	public:
		VulkanCommandList(uint32_t queue_family, DeviceApiTable const& in_api) : api(&in_api)
		{
			VkCommandPoolCreateInfo const pool_info {
				.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.queueFamilyIndex = queue_family,
			};
			auto result = api->vkCreateCommandPool(api->device, &pool_info, nullptr, std::out_ptr(pool, *api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan command pool.");

			VkCommandBufferAllocateInfo const alloc_info {
				.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool		= pool.get(),
				.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};
			result = api->vkAllocateCommandBuffers(api->device, &alloc_info, &cmd);
			VT_CHECK_RESULT(result, "Failed to allocate Vulkan command buffer.");
		}

		CommandListHandle get_handle()
		{
			return {cmd};
		}

		void reset()
		{
			auto result = api->vkResetCommandPool(api->device, pool.get(), 0);
			VT_CHECK_RESULT(result, "Failed to reset Vulkan command pool.");
		}

		void begin()
		{
			VkCommandBufferBeginInfo const begin_info {
				.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				.flags			  = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
				.pInheritanceInfo = nullptr,
			};
			auto result = api->vkBeginCommandBuffer(cmd, &begin_info);
			VT_CHECK_RESULT(result, "Failed to begin Vulkan command buffer.");
		}

		void end()
		{
			auto result = api->vkEndCommandBuffer(cmd);
			VT_CHECK_RESULT(result, "Failed to end Vulkan command buffer.");
		}

		void copy_buffer(Buffer const& src, Buffer& dst)
		{
			VkBufferCopy const copy {
				.srcOffset = 0,
				.dstOffset = 0,
				.size	   = src.get_size(),
			};
			api->vkCmdCopyBuffer(cmd, src.vulkan.get_handle(), dst.vulkan.get_handle(), 1, &copy);
		}

		void copy_image(Image const& src, Image& dst)
		{
			VkImageCopy const copy {
				.srcSubresource {
					.aspectMask		= IMAGE_ASPECT_FLAGS_LOOKUP[src.get_format()],
					.mipLevel		= 0,
					.baseArrayLayer = 0,
					.layerCount		= 1,
				},
				.srcOffset = {0, 0, 0},
				.dstSubresource {
					.aspectMask		= IMAGE_ASPECT_FLAGS_LOOKUP[dst.get_format()],
					.mipLevel		= 0,
					.baseArrayLayer = 0,
					.layerCount		= 1,
				},
				.dstOffset = {0, 0, 0},
				.extent {
					.width	= src.get_width(),
					.height = src.get_height(),
					.depth	= src.get_depth(),
				},
			};
			api->vkCmdCopyImage(cmd, src.vulkan.get_handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst.vulkan.get_handle(),
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		}

		void copy_buffer_to_image(Buffer const& src, Image& dst)
		{
			VkBufferImageCopy const copy {
				.bufferOffset	   = 0,
				.bufferRowLength   = 0, // Zero here means the layout is the same as the destination image extent.
				.bufferImageHeight = 0,
				.imageSubresource {
					.aspectMask		= IMAGE_ASPECT_FLAGS_LOOKUP[dst.get_format()],
					.mipLevel		= 0,
					.baseArrayLayer = 0,
					.layerCount		= 1,
				},
				.imageOffset = {0, 0, 0},
				.imageExtent {
					.width	= dst.get_width(),
					.height = dst.get_height(),
					.depth	= dst.get_depth(),
				},
			};
			api->vkCmdCopyBufferToImage(cmd, src.vulkan.get_handle(), dst.vulkan.get_handle(),
										VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		}

		void update_buffer(Buffer& dst, size_t offset, size_t size, void const* data)
		{
			api->vkCmdUpdateBuffer(cmd, dst.vulkan.get_handle(), offset, size, data);
		}

		void copy_buffer_region(Buffer const& src, Buffer& dst, size_t src_offset, size_t dst_offset, size_t size)
		{
			VkBufferCopy const copy {
				.srcOffset = src_offset,
				.dstOffset = dst_offset,
				.size	   = size,
			};
			api->vkCmdCopyBuffer(cmd, src.vulkan.get_handle(), dst.vulkan.get_handle(), 1, &copy);
		}

		void copy_image_region(Image const& src, Image& dst, ImageCopyRegion const& region)
		{
			VkImageCopy const copy {
				.srcSubresource {
					.aspectMask		= IMAGE_ASPECT_FLAGS_LOOKUP[src.get_format()],
					.mipLevel		= region.src_mip,
					.baseArrayLayer = region.src_array_index,
					.layerCount		= 1,
				},
				.srcOffset {
					.x = static_cast<int>(region.src_offset.x),
					.y = static_cast<int>(region.src_offset.y),
					.z = static_cast<int>(region.src_offset.z),
				},
				.dstSubresource {
					.aspectMask		= IMAGE_ASPECT_FLAGS_LOOKUP[dst.get_format()],
					.mipLevel		= region.dst_mip,
					.baseArrayLayer = region.dst_array_index,
					.layerCount		= 1,
				},
				.dstOffset {
					.x = static_cast<int>(region.dst_offset.x),
					.y = static_cast<int>(region.dst_offset.y),
					.z = static_cast<int>(region.dst_offset.z),
				},
				.extent {
					.width	= region.expanse.width,
					.height = region.expanse.height,
					.depth	= region.expanse.depth,
				},
			};
			api->vkCmdCopyImage(cmd, src.vulkan.get_handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst.vulkan.get_handle(),
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		}

		void bind_compute_pipeline(ComputePipeline const& pipeline)
		{
			api->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.vulkan.get_handle());
		}

		void bind_compute_root_signature(RootSignature const& root_signature)
		{
			this->bound_compute_layout = root_signature.vulkan.get_data_for_command_list();
		}

		void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			auto& signature = this->bound_compute_layout;
			auto  layout	= signature.get_compute_layout_handle();
			bind_descriptor_sets(VK_PIPELINE_BIND_POINT_COMPUTE, signature, layout, descriptor_sets);
		}

		void push_compute_constants(size_t byte_offset, size_t byte_size, void const* data)
		{
			auto	 layout = this->bound_compute_layout.get_compute_layout_handle();
			uint32_t offset = static_cast<uint32_t>(byte_offset);
			uint32_t size	= static_cast<uint32_t>(byte_size);
			api->vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_COMPUTE_BIT, offset, size, data);
		}

		void dispatch(unsigned x_count, unsigned y_count, unsigned z_count)
		{
			api->vkCmdDispatch(cmd, x_count, y_count, z_count);
		}

		void dispatch_indirect(Buffer const& buffer, size_t offset)
		{
			api->vkCmdDispatchIndirect(cmd, buffer.vulkan.get_handle(), offset);
		}

		void begin_render_pass(RenderPass const&	 render_pass,
							   RenderTarget const&	 render_target,
							   ConstSpan<ClearValue> clear_values = {})
		{
			VkRenderPassBeginInfo const begin_info {
				.sType		 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				.renderPass	 = render_pass.vulkan.get_handle(),
				.framebuffer = render_target.vulkan.get_handle(),
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
			// static_assert(std::is_layout_compatible_v<VkClearValue, ClearValue>); // TODO: Wait for compiler fix

			api->vkCmdBeginRenderPass(cmd, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
		}

		void change_subpass()
		{
			api->vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);
		}

		void end_render_pass()
		{
			api->vkCmdEndRenderPass(cmd);
		}

		void bind_render_pipeline(RenderPipeline const& pipeline)
		{
			api->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vulkan.get_handle());
		}

		void bind_render_root_signature(RootSignature const& root_signature)
		{
			this->bound_render_layout = root_signature.vulkan.get_data_for_command_list();
		}

		void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			auto& signature = this->bound_render_layout;
			auto  layout	= signature.get_render_layout_handle();
			bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, signature, layout, descriptor_sets);
		}

		void push_render_constants(size_t byte_offset, size_t byte_size, void const* data)
		{
			auto	 layout = this->bound_render_layout.get_render_layout_handle();
			auto	 stages = this->bound_render_layout.get_render_push_constant_stages();
			uint32_t offset = static_cast<uint32_t>(byte_offset);
			uint32_t size	= static_cast<uint32_t>(byte_size);
			api->vkCmdPushConstants(cmd, layout, stages, offset, size, data);
		}

		void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets)
		{
			FixedList<VkBuffer, MAX_VERTEX_BUFFERS> handles(buffers.size());

			auto handle = handles.begin();
			for(auto& buffer : buffers)
				*handle++ = buffer.vulkan.get_handle();

			api->vkCmdBindVertexBuffers(cmd, first_buffer, count(handles), handles.data(), byte_offsets.data());
		}

		void bind_index_buffer(Buffer const& buffer, size_t byte_offset)
		{
			auto index_type = get_index_type_from_stride(buffer.get_stride());
			api->vkCmdBindIndexBuffer(cmd, buffer.vulkan.get_handle(), byte_offset, index_type);
		}

		void set_viewports(ArrayView<Viewport> viewports)
		{
			FixedList<VkViewport, MAX_ATTACHMENTS> vk_viewports(viewports.size());

			auto viewport = vk_viewports.begin();
			for(auto vp : viewports)
				*viewport++ = VkViewport {
					.x		  = vp.x,
					.y		  = vp.height + vp.y,
					.width	  = vp.width,
					.height	  = -vp.height,
					.minDepth = vp.min_depth,
					.maxDepth = vp.max_depth,
				};

			api->vkCmdSetViewport(cmd, 0, count(vk_viewports), vk_viewports.data());
		}

		void set_scissors(ArrayView<Rectangle> scissors)
		{
			// static_assert(std::is_layout_compatible_v<Rectangle, VkRect2D>); // TODO: Wait for compiler fix

			auto data = reinterpret_cast<VkRect2D const*>(scissors.data());
			api->vkCmdSetScissor(cmd, 0, count(scissors), data);
		}

		void set_blend_constants(Float4 blend_constants)
		{
			api->vkCmdSetBlendConstants(cmd, &blend_constants[0]);
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

		void draw_indirect(Buffer const& buffer, size_t offset, unsigned draws)
		{
			api->vkCmdDrawIndirect(cmd, buffer.vulkan.get_handle(), offset, draws, sizeof(VkDrawIndirectCommand));
		}

		void draw_indexed_indirect(Buffer const& buffer, size_t offset, unsigned draws)
		{
			api->vkCmdDrawIndexedIndirect(cmd, buffer.vulkan.get_handle(), offset, draws, sizeof(VkDrawIndexedIndirectCommand));
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

		void bind_descriptor_sets(VkPipelineBindPoint				   bind_point,
								  CommandListPipelineLayoutData const& root_signature,
								  VkPipelineLayout					   layout,
								  ArrayView<DescriptorSet>			   descriptor_sets) const
		{
			SmallList<VkDescriptorSet> sets;
			sets.reserve(descriptor_sets.size());

			sets.emplace_back(descriptor_sets[0].vulkan.get_handle());
			uint32_t start_index = root_signature.get_layout_index(descriptor_sets[0].vulkan.get_layout());
			uint32_t prev_index	 = start_index;

			for(auto& set : descriptor_sets | std::views::drop(1))
			{
				uint32_t index = root_signature.get_layout_index(set.vulkan.get_layout());
				if(index != prev_index + 1)
				{
					api->vkCmdBindDescriptorSets(cmd, bind_point, layout, start_index, count(sets), sets.data(), 0, nullptr);
					sets.clear();
					start_index = index;
				}
				sets.emplace_back(set.vulkan.get_handle());
				++prev_index;
			}
			api->vkCmdBindDescriptorSets(cmd, bind_point, layout, start_index, count(sets), sets.data(), 0, nullptr);
		}
	};
}
