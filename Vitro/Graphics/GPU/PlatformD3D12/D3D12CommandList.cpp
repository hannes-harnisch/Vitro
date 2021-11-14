module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <cstdlib>
#include <type_traits>
export module vt.Graphics.D3D12.CommandList;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.LookupTable;
import vt.Core.Rect;
import vt.Core.SmallList;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.RenderPass;
import vt.Graphics.D3D12.RenderTarget;
import vt.Graphics.D3D12.RootSignature;
import vt.Graphics.AssetResource;
import vt.Graphics.CommandListBase;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Handle;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;

namespace vt::d3d12
{
	constexpr inline auto COMMAND_TYPE_LOOKUP = [] {
		LookupTable<CommandType, D3D12_COMMAND_LIST_TYPE> _;
		using enum CommandType;

		_[Copy]	   = D3D12_COMMAND_LIST_TYPE_COPY;
		_[Compute] = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		_[Render]  = D3D12_COMMAND_LIST_TYPE_DIRECT;
		return _;
	}();

	template<CommandType> class CommandListData;

	template<> class CommandListData<CommandType::Copy>
	{};

	template<> class CommandListData<CommandType::Compute> : protected CommandListData<CommandType::Copy>
	{
	protected:
		ID3D12DescriptorHeap*	view_heap;
		ID3D12DescriptorHeap*	sampler_heap;
		RootParameterMap		bound_compute_root_indices;
		ID3D12CommandSignature* dispatch_signature;
	};

	template<> class CommandListData<CommandType::Render> : protected CommandListData<CommandType::Compute>
	{
	protected:
		D3D12_CPU_DESCRIPTOR_HANDLE			   rtv_null_descriptor;
		ID3D12CommandSignature*				   draw_signature;
		ID3D12CommandSignature*				   draw_indexed_signature;
		RootParameterMap					   bound_render_root_indices;
		CommandListRenderPassData			   bound_render_pass;
		CommandListRenderTargetData			   bound_render_target;
		D3D_PRIMITIVE_TOPOLOGY				   bound_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		unsigned							   subpass_index;
		FixedList<ClearValue, MAX_ATTACHMENTS> clear_values;
	};

	export template<CommandType TYPE> class D3D12CommandList final : public CommandListBase<TYPE>, private CommandListData<TYPE>
	{
	public:
		D3D12CommandList(ID3D12Device4&			 device,
						 DescriptorPool const&	 descriptor_pool,
						 ID3D12CommandSignature* dispatch_signature,
						 ID3D12CommandSignature* draw_signature,
						 ID3D12CommandSignature* draw_indexed_signature)
		{
			if constexpr(TYPE != CommandType::Copy)
			{
				this->view_heap			 = descriptor_pool.get_shader_visible_view_heap();
				this->sampler_heap		 = descriptor_pool.get_shader_visible_sampler_heap();
				this->dispatch_signature = dispatch_signature;
			}
			if constexpr(TYPE == CommandType::Render)
			{
				this->rtv_null_descriptor	 = descriptor_pool.get_rtv_null_descriptor();
				this->draw_signature		 = draw_signature;
				this->draw_indexed_signature = draw_indexed_signature;
			}

			auto result = device.CreateCommandAllocator(COMMAND_TYPE_LOOKUP[TYPE], VT_COM_OUT(allocator));
			VT_CHECK_RESULT(result, "Failed to create D3D12 command allocator.");

			result = device.CreateCommandList1(0, COMMAND_TYPE_LOOKUP[TYPE], D3D12_COMMAND_LIST_FLAG_NONE, VT_COM_OUT(cmd));
			VT_CHECK_RESULT(result, "Failed to create D3D12 command list.");
		}

		CommandListHandle get_handle()
		{
			return {cmd.get()};
		}

		void reset()
		{
			auto result = allocator->Reset();
			VT_CHECK_RESULT(result, "Failed to reset D3D12 command allocator.");
		}

		void begin()
		{
			auto result = cmd->Reset(allocator.get(), nullptr);
			VT_CHECK_RESULT(result, "Failed to reset D3D12 command list.");

			if constexpr(TYPE != CommandType::Copy)
			{
				ID3D12DescriptorHeap* heaps[] {
					this->view_heap,
					this->sampler_heap,
				};
				cmd->SetDescriptorHeaps(count(heaps), heaps);
			}
		}

		void end()
		{
			auto result = cmd->Close();
			VT_CHECK_RESULT(result, "Failed to end D3D12 command list.");

			if constexpr(TYPE == CommandType::Render)
				this->bound_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}

		void copy_buffer(Buffer const& src, Buffer& dst)
		{
			cmd->CopyResource(dst.d3d12.get_resource(), src.d3d12.get_resource());
		}

		void copy_image(Image const& src, Image& dst)
		{
			cmd->CopyResource(dst.d3d12.get_resource(), src.d3d12.get_resource());
		}

		void copy_buffer_to_image(Buffer const& src, Image& dst)
		{
			cmd->CopyResource(dst.d3d12.get_resource(), src.d3d12.get_resource());
		}

		void update_buffer(Buffer& dst, size_t offset, size_t size, void const* data)
		{
			SmallList<D3D12_WRITEBUFFERIMMEDIATE_PARAMETER> params(size / sizeof(DWORD));

			auto param	 = params.begin();
			auto address = dst.d3d12.get_gpu_address() + offset;
			auto end	 = address + size;
			auto ptr	 = static_cast<char const*>(data);
			while(address != end)
			{
				param->Dest = address;
				std::memcpy(&param->Value, ptr, sizeof(DWORD));

				address += sizeof(DWORD);
				ptr += sizeof(DWORD);
				++param;
			}
			cmd->WriteBufferImmediate(count(params), params.data(), nullptr);
		}

		void copy_buffer_region(Buffer const& src, Buffer& dst, size_t src_offset, size_t dst_offset, size_t size)
		{
			cmd->CopyBufferRegion(dst.d3d12.get_resource(), dst_offset, src.d3d12.get_resource(), src_offset, size);
		}

		void copy_image_region(Image const& src, Image& dst, ImageCopyRegion const& region)
		{
			D3D12_TEXTURE_COPY_LOCATION const source {
				.pResource		  = src.d3d12.get_resource(),
				.Type			  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				.SubresourceIndex = region.src_mip + region.src_array_index * src.count_mips(),
			};
			D3D12_TEXTURE_COPY_LOCATION const destination {
				.pResource		  = dst.d3d12.get_resource(),
				.Type			  = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
				.SubresourceIndex = region.dst_mip + region.dst_array_index * dst.count_mips(),
			};
			D3D12_BOX const box {
				.left	= region.src_offset.x,
				.top	= region.src_offset.y,
				.front	= region.src_offset.z,
				.right	= box.left + region.expanse.width,
				.bottom = box.top + region.expanse.height,
				.back	= box.front + region.expanse.depth,
			};
			UINT dst_x = region.dst_offset.x;
			UINT dst_y = region.dst_offset.y;
			UINT dst_z = region.dst_offset.z;
			cmd->CopyTextureRegion(&destination, dst_x, dst_y, dst_z, &source, &box);
		}

		void bind_compute_pipeline(ComputePipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.get_handle());
		}

		void bind_compute_root_signature(RootSignature const& root_signature)
		{
			cmd->SetComputeRootSignature(root_signature.d3d12.get_handle());
			this->bound_compute_root_indices = root_signature.d3d12.get_parameter_map();
		}

		void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			bind_descriptor_sets<false>(descriptor_sets, this->bound_compute_root_indices);
		}

		void push_compute_constants(size_t byte_offset, size_t byte_size, void const* data)
		{
			VT_ASSERT(byte_offset % sizeof(DWORD) == 0, "Byte offset must be divisible by 4.");
			VT_ASSERT(byte_size % sizeof(DWORD) == 0, "Byte size must be divisible by 4.");

			UINT dword_size	  = static_cast<UINT>(byte_size / sizeof(DWORD));
			UINT dword_offset = static_cast<UINT>(byte_offset / sizeof(DWORD));
			cmd->SetComputeRoot32BitConstants(0, dword_size, data, dword_offset);
		}

		void dispatch(unsigned x_count, unsigned y_count, unsigned z_count)
		{
			cmd->Dispatch(x_count, y_count, z_count);
		}

		void dispatch_indirect(Buffer const& buffer, size_t offset)
		{
			cmd->ExecuteIndirect(this->dispatch_signature, 1, buffer.d3d12.get_resource(), offset, nullptr, 0);
		}

		void begin_render_pass(RenderPass const&	 render_pass,
							   RenderTarget const&	 render_target,
							   ConstSpan<ClearValue> clear_values = {})
		{
			this->bound_render_pass	  = render_pass.d3d12.get_data_for_command_list();
			this->bound_render_target = render_target.d3d12.get_data_for_command_list();
			this->clear_values.assign(clear_values.begin(), clear_values.end());

			begin_subpass(0);
			this->subpass_index = 1;
		}

		void change_subpass()
		{
			VT_ASSERT(this->subpass_index < this->bound_render_pass.count_subpasses() - 1,
					  "All subpasses of this render pass have already been transitioned through.");

			cmd->EndRenderPass();
			begin_subpass(this->subpass_index++);
		}

		void end_render_pass()
		{
			cmd->EndRenderPass();

			auto& final_transitions = this->bound_render_pass.get_final_transitions();

			FixedList<D3D12_RESOURCE_BARRIER, MAX_ATTACHMENTS> barriers(final_transitions.size());

			auto barrier = barriers.begin();
			for(auto transition : final_transitions)
			{
				*barrier++ = D3D12_RESOURCE_BARRIER {
					.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
					.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
					.Transition {
						.pResource	 = this->bound_render_target.get_attachment(transition.index),
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = transition.old_layout,
						.StateAfter	 = transition.new_layout,
					},
				};
			}
			cmd->ResourceBarrier(count(barriers), barriers.data());
		}

		void bind_render_pipeline(RenderPipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.get_handle());

			auto pipeline_topology = pipeline.d3d12.get_topology();
			if(this->bound_primitive_topology != pipeline_topology)
			{
				cmd->IASetPrimitiveTopology(pipeline_topology);
				this->bound_primitive_topology = pipeline_topology;
			}
		}

		void bind_render_root_signature(RootSignature const& root_signature)
		{
			cmd->SetGraphicsRootSignature(root_signature.d3d12.get_handle());
			this->bound_render_root_indices = root_signature.d3d12.get_parameter_map();
		}

		void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			bind_descriptor_sets<true>(descriptor_sets, this->bound_render_root_indices);
		}

		void push_render_constants(size_t byte_offset, size_t byte_size, void const* data)
		{
			VT_ASSERT(byte_offset % sizeof(DWORD) == 0, "Byte offset must be divisible by 4.");
			VT_ASSERT(byte_size % sizeof(DWORD) == 0, "Byte size must be divisible by 4.");

			UINT dword_size	  = static_cast<UINT>(byte_size / sizeof(DWORD));
			UINT dword_offset = static_cast<UINT>(byte_offset / sizeof(DWORD));
			cmd->SetGraphicsRoot32BitConstants(0, dword_size, data, dword_offset);
		}

		void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets)
		{
			FixedList<D3D12_VERTEX_BUFFER_VIEW, MAX_VERTEX_BUFFERS> views(buffers.size());

			auto view	= views.begin();
			auto offset = byte_offsets.begin();
			for(auto& buffer : buffers)
				*view++ = D3D12_VERTEX_BUFFER_VIEW {
					.BufferLocation = buffer.d3d12.get_gpu_address() + *offset++,
					.SizeInBytes	= buffer.get_size(),
					.StrideInBytes	= buffer.get_stride(),
				};

			cmd->IASetVertexBuffers(first_buffer, count(views), views.data());
		}

		void bind_index_buffer(Buffer const& buffer, size_t byte_offset)
		{
			D3D12_INDEX_BUFFER_VIEW const view {
				.BufferLocation = buffer.d3d12.get_gpu_address() + byte_offset,
				.SizeInBytes	= buffer.get_size(),
				.Format			= get_index_format_from_stride(buffer.get_stride()),
			};
			cmd->IASetIndexBuffer(&view);
		}

		void set_viewports(ArrayView<Viewport> viewports)
		{
			static_assert(std::is_layout_compatible_v<Viewport, D3D12_VIEWPORT>);

			auto data = reinterpret_cast<D3D12_VIEWPORT const*>(viewports.data());
			cmd->RSSetViewports(count(viewports), data);
		}

		void set_scissors(ArrayView<Rectangle> scissors)
		{
			FixedList<D3D12_RECT, MAX_ATTACHMENTS> rects(scissors.size());

			auto rect = rects.begin();
			for(auto scissor : scissors)
				*rect++ = D3D12_RECT {
					.left	= scissor.x,
					.top	= scissor.y,
					.right	= static_cast<LONG>(scissor.x + scissor.width),
					.bottom = static_cast<LONG>(scissor.y + scissor.height),
				};

			cmd->RSSetScissorRects(count(rects), rects.data());
		}

		void set_blend_constants(Float4 blend_constants)
		{
			cmd->OMSetBlendFactor(&blend_constants[0]);
		}

		void set_depth_bounds(float min, float max)
		{
			cmd->OMSetDepthBounds(min, max);
		}

		void set_stencil_reference(unsigned reference_value)
		{
			cmd->OMSetStencilRef(reference_value);
		}

		void draw(unsigned vertex_count, unsigned instance_count, unsigned first_vertex, unsigned first_instance)
		{
			cmd->DrawInstanced(vertex_count, instance_count, first_vertex, first_instance);
		}

		void draw_indexed(unsigned index_count,
						  unsigned instance_count,
						  unsigned first_index,
						  int	   vertex_offset,
						  unsigned first_instance)
		{
			cmd->DrawIndexedInstanced(index_count, instance_count, first_index, vertex_offset, first_instance);
		}

		void draw_indirect(Buffer const& buffer, size_t offset, unsigned draws)
		{
			cmd->ExecuteIndirect(this->draw_signature, draws, buffer.d3d12.get_resource(), offset, nullptr, 0);
		}

		void draw_indexed_indirect(Buffer const& buffer, size_t offset, unsigned draws)
		{
			cmd->ExecuteIndirect(this->draw_indexed_signature, draws, buffer.d3d12.get_resource(), offset, nullptr, 0);
		}

	private:
		ComUnique<ID3D12CommandAllocator>	  allocator;
		ComUnique<ID3D12GraphicsCommandList4> cmd;

		static DXGI_FORMAT get_index_format_from_stride(unsigned stride)
		{
			switch(stride)
			{
				case 2: return DXGI_FORMAT_R16_UINT;
				case 4: return DXGI_FORMAT_R32_UINT;
			}
			VT_UNREACHABLE();
		}

		void insert_subpass_barriers(TransitionList const& transitions) const
		{
			FixedList<D3D12_RESOURCE_BARRIER, MAX_ATTACHMENTS> barriers(transitions.size());

			auto barrier = barriers.begin();
			for(auto transition : transitions)
			{
				auto resource  = this->bound_render_target.get_attachment(transition.index);
				barrier->Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				if(transition.requires_uav_barrier)
				{
					barrier->Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
					barrier->UAV  = {
						 .pResource = resource,
					 };
				}
				else
				{
					barrier->Type		= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier->Transition = {
						.pResource	 = resource,
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = transition.old_layout,
						.StateAfter	 = transition.new_layout,
					};
				};
				++barrier;
			}
			cmd->ResourceBarrier(count(barriers), barriers.data());
		}

		void begin_subpass(unsigned subpass_index) const
		{
			auto& pass	  = this->bound_render_pass;
			auto& target  = this->bound_render_target;
			auto& clears  = this->clear_values;
			auto& subpass = pass.get_subpass(subpass_index);
			insert_subpass_barriers(subpass.transitions);

			FixedList<D3D12_RENDER_PASS_RENDER_TARGET_DESC, MAX_COLOR_ATTACHMENTS> rt_descs(pass.count_color_attachments());

			auto rt_desc = rt_descs.begin();
			for(size_t index = 0; auto access : subpass.get_color_attachment_accesses())
			{
				D3D12_CPU_DESCRIPTOR_HANDLE rt_descriptor;
				D3D12_CLEAR_VALUE			color_clear_value = {};
				if(access.for_input)
					rt_descriptor = this->rtv_null_descriptor;
				else
				{
					rt_descriptor = target.get_render_target_view(index);
					if(!clears.empty())
						color_clear_value = {
							.Format = pass.get_attachment_format(index),
							.Color {
								clears[index].color.r,
								clears[index].color.g,
								clears[index].color.b,
								clears[index].color.a,
							},
						};
				}

				*rt_desc++ = D3D12_RENDER_PASS_RENDER_TARGET_DESC {
					.cpuDescriptor = rt_descriptor,
					.BeginningAccess {
						.Type = access.begin,
						.Clear {
							.ClearValue = color_clear_value,
						},
					},
					.EndingAccess {
						.Type = access.end,
					},
				};
				++index;
			}

			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC ds_desc;
			if(subpass.uses_depth_stencil())
			{
				D3D12_CLEAR_VALUE depth_clear_value, stencil_clear_value;
				if(clears.empty())
				{
					depth_clear_value	= {};
					stencil_clear_value = {};
				}
				else
				{
					depth_clear_value = {
						.Format = pass.get_depth_stencil_format(),
						.DepthStencil {
							.Depth = clears.back().depth,
						},
					};
					stencil_clear_value = {
						.Format = pass.get_depth_stencil_format(),
						.DepthStencil {
							.Stencil = static_cast<UINT8>(clears.back().stencil),
						},
					};
				}

				ds_desc = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC {
					.cpuDescriptor = target.get_depth_stencil_view(),
					.DepthBeginningAccess {
						.Type = subpass.get_depth_begin_access(),
						.Clear {
							.ClearValue = depth_clear_value,
						},
					},
					.StencilBeginningAccess {
						.Type = subpass.stencil_begin,
						.Clear {
							.ClearValue = stencil_clear_value,
						},
					},
					.DepthEndingAccess {
						.Type = subpass.get_depth_end_access(),
					},
					.StencilEndingAccess {
						.Type = subpass.stencil_end,
					},
				};
			}
			auto ds_desc_ptr = subpass.uses_depth_stencil() ? &ds_desc : nullptr;
			cmd->BeginRenderPass(count(rt_descs), rt_descs.data(), ds_desc_ptr, subpass.flags);
		}

		template<bool RENDER> void bind_descriptor_sets(ArrayView<DescriptorSet> sets, RootParameterMap const& indices)
		{
			for(auto& set : sets)
			{
				auto [view_index, sampler_table_index] = indices.find(set.d3d12.get_layout_id());
				switch(set.d3d12.get_view_parameter_type())
				{
					case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
						bind_descriptor_table<RENDER>(view_index, set.d3d12.get_view_table_start());
						break;
					case D3D12_ROOT_PARAMETER_TYPE_CBV: bind_cbv<RENDER>(view_index, set.d3d12.get_gpu_address()); break;
					case D3D12_ROOT_PARAMETER_TYPE_SRV: bind_srv<RENDER>(view_index, set.d3d12.get_gpu_address()); break;
					case D3D12_ROOT_PARAMETER_TYPE_UAV: bind_uav<RENDER>(view_index, set.d3d12.get_gpu_address()); break;
				}
				if(sampler_table_index)
					bind_descriptor_table<RENDER>(sampler_table_index, set.d3d12.get_sampler_table_start());
			}
		}

		template<bool RENDER> void bind_descriptor_table(UINT index, D3D12_GPU_DESCRIPTOR_HANDLE table)
		{
			if constexpr(RENDER)
				cmd->SetGraphicsRootDescriptorTable(index, table);
			else
				cmd->SetComputeRootDescriptorTable(index, table);
		}

		template<bool RENDER> void bind_cbv(UINT index, D3D12_GPU_VIRTUAL_ADDRESS view)
		{
			if constexpr(RENDER)
				cmd->SetGraphicsRootConstantBufferView(index, view);
			else
				cmd->SetComputeRootConstantBufferView(index, view);
		}

		template<bool RENDER> void bind_srv(UINT index, D3D12_GPU_VIRTUAL_ADDRESS view)
		{
			if constexpr(RENDER)
				cmd->SetGraphicsRootShaderResourceView(index, view);
			else
				cmd->SetComputeRootShaderResourceView(index, view);
		}

		template<bool RENDER> void bind_uav(UINT index, D3D12_GPU_VIRTUAL_ADDRESS view)
		{
			if constexpr(RENDER)
				cmd->SetGraphicsRootUnorderedAccessView(index, view);
			else
				cmd->SetComputeRootUnorderedAccessView(index, view);
		}
	};
}
