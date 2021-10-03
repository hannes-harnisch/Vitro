module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <cstdlib>
#include <ranges>
#include <type_traits>
export module vt.Graphics.D3D12.CommandList;

import vt.Core.Algorithm;
import vt.Core.FixedList;
import vt.Core.Rectangle;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.RenderPass;
import vt.Graphics.D3D12.RenderTarget;
import vt.Graphics.D3D12.RootSignature;
import vt.Graphics.AssetResource;
import vt.Graphics.CommandListBase;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Device;
import vt.Graphics.Handle;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;

namespace vt::d3d12
{
	constexpr D3D12_COMMAND_LIST_TYPE convert_command_type(CommandType type)
	{
		using enum CommandType;
		switch(type)
		{
			case Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
			case Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case Render: return D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		VT_UNREACHABLE();
	}

	template<CommandType> class CommandListData;

	template<> class CommandListData<CommandType::Copy>
	{};

	template<> class CommandListData<CommandType::Compute> : protected CommandListData<CommandType::Copy>
	{
	protected:
		RootSignatureParameterMap const* active_compute_root_indices = nullptr;
	};

	template<> class CommandListData<CommandType::Render> : protected CommandListData<CommandType::Compute>
	{
	protected:
		D3D12RenderPass const*			 active_render_pass			= nullptr;
		D3D12RenderTarget const*		 active_render_target		= nullptr;
		RootSignatureParameterMap const* active_render_root_indices = nullptr;
		unsigned						 subpass_index				= 1;
	};

	export template<CommandType TYPE> class D3D12CommandList final : public CommandListBase<TYPE>, private CommandListData<TYPE>
	{
	public:
		D3D12CommandList(Device& in_device) : device(&in_device.d3d12)
		{
			ID3D12CommandAllocator* raw_allocator;

			auto dev_ptr = device->ptr();
			auto result	 = dev_ptr->CreateCommandAllocator(COMMAND_LIST_TYPE, IID_PPV_ARGS(&raw_allocator));
			allocator.reset(raw_allocator);
			VT_ASSERT_RESULT(result, "Failed to create D3D12 command allocator.");

			ID3D12GraphicsCommandList4* raw_cmd;
			result = dev_ptr->CreateCommandList1(0, COMMAND_LIST_TYPE, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&raw_cmd));
			cmd.reset(raw_cmd);
			VT_ASSERT_RESULT(result, "Failed to create D3D12 command list.");
		}

		CommandListHandle handle()
		{
			return {cmd.get()};
		}

		void reset()
		{
			auto result = allocator->Reset();
			VT_ASSERT_RESULT(result, "Failed to reset D3D12 command allocator.");
		}

		void begin()
		{
			auto result = cmd->Reset(allocator.get(), nullptr);
			VT_ASSERT_RESULT(result, "Failed to reset D3D12 command list.");

			auto heaps = device->get_shader_visible_descriptor_heaps();
			cmd->SetDescriptorHeaps(2, heaps.data());
		}

		void end()
		{
			auto result = cmd->Close();
			VT_ASSERT_RESULT(result, "Failed to end D3D12 command list.");
		}

		void bind_compute_pipeline(ComputePipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.ptr());
		}

		void bind_compute_root_signature(RootSignature const& root_signature)
		{
			cmd->SetComputeRootSignature(root_signature.d3d12.ptr());
			this->active_compute_root_indices = &root_signature.d3d12.get_parameter_map();
		}

		void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			for(auto& set : descriptor_sets)
			{
				unsigned index = this->active_compute_root_indices->find(set.d3d12.get_layout_id());
				switch(set.d3d12.get_parameter_type())
				{
					case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
						cmd->SetComputeRootDescriptorTable(index, set.d3d12.get_table_handle());
						break;
					case D3D12_ROOT_PARAMETER_TYPE_CBV:
						cmd->SetComputeRootConstantBufferView(index, set.d3d12.get_gpu_address());
						break;
					case D3D12_ROOT_PARAMETER_TYPE_SRV:
						cmd->SetComputeRootShaderResourceView(index, set.d3d12.get_gpu_address());
						break;
					case D3D12_ROOT_PARAMETER_TYPE_UAV:
						cmd->SetComputeRootUnorderedAccessView(index, set.d3d12.get_gpu_address());
						break;
				}
			}
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

		void bind_render_pipeline(RenderPipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.ptr());
		}

		void bind_render_root_signature(RootSignature const& root_signature)
		{
			cmd->SetGraphicsRootSignature(root_signature.d3d12.ptr());
			this->active_render_root_indices = &root_signature.d3d12.get_parameter_map();
		}

		void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			for(auto& set : descriptor_sets)
			{
				unsigned index = this->active_render_root_indices->find(set.d3d12.get_layout_id());
				switch(set.d3d12.get_parameter_type())
				{
					case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
						cmd->SetGraphicsRootDescriptorTable(index, set.d3d12.get_table_handle());
						break;
					case D3D12_ROOT_PARAMETER_TYPE_CBV:
						cmd->SetGraphicsRootConstantBufferView(index, set.d3d12.get_gpu_address());
						break;
					case D3D12_ROOT_PARAMETER_TYPE_SRV:
						cmd->SetGraphicsRootShaderResourceView(index, set.d3d12.get_gpu_address());
						break;
					case D3D12_ROOT_PARAMETER_TYPE_UAV:
						cmd->SetGraphicsRootUnorderedAccessView(index, set.d3d12.get_gpu_address());
						break;
				}
			}
		}

		void push_render_constants(size_t byte_offset, size_t byte_size, void const* data)
		{
			VT_ASSERT(byte_offset % sizeof(DWORD) == 0, "Byte offset must be divisible by 4.");
			VT_ASSERT(byte_size % sizeof(DWORD) == 0, "Byte size must be divisible by 4.");

			UINT dword_size	  = static_cast<UINT>(byte_size / sizeof(DWORD));
			UINT dword_offset = static_cast<UINT>(byte_offset / sizeof(DWORD));
			cmd->SetGraphicsRoot32BitConstants(0, dword_size, data, dword_offset);
		}

		void begin_render_pass(RenderPass const&	 render_pass,
							   RenderTarget const&	 render_target,
							   ConstSpan<ClearValue> clear_values = {})
		{
			this->active_render_pass   = &render_pass.d3d12;
			this->active_render_target = &render_target.d3d12;

			auto& pass	 = *this->active_render_pass;
			auto& target = *this->active_render_target;
			insert_render_pass_barriers(pass.get_subpass_transitions(0));

			FixedList<D3D12_RENDER_PASS_RENDER_TARGET_DESC, MAX_COLOR_ATTACHMENTS> rt_descs;

			unsigned index = 0;
			for(auto attachment : pass.get_render_target_attachments())
			{
				D3D12_CLEAR_VALUE color_clear;
				if(clear_values.empty())
					color_clear = {};
				else
					color_clear = {
						.Format = attachment.format,
						.Color {
							clear_values[index].color.r,
							clear_values[index].color.g,
							clear_values[index].color.b,
							clear_values[index].color.a,
						},
					};

				rt_descs.emplace_back(D3D12_RENDER_PASS_RENDER_TARGET_DESC {
					.cpuDescriptor = target.get_view(index),
					.BeginningAccess {
						.Type  = attachment.begin_access,
						.Clear = {color_clear},
					},
					.EndingAccess {
						.Type = attachment.end_access,
					},
				});
				++index;
			}

			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC ds_desc;
			if(pass.uses_depth_stencil())
			{
				D3D12_CLEAR_VALUE depth_clear, stencil_clear;
				if(clear_values.empty())
				{
					depth_clear	  = {};
					stencil_clear = {};
				}
				else
				{
					depth_clear = {
						.Format = pass.get_depth_stencil_format(),
						.DepthStencil {
							.Depth = clear_values.back().depth,
						},
					};
					stencil_clear = {
						.Format = pass.get_depth_stencil_format(),
						.DepthStencil {
							.Stencil = static_cast<UINT8>(clear_values.back().stencil),
						},
					};
				}

				ds_desc = {
					.cpuDescriptor = target.get_depth_stencil_view(),
					.DepthBeginningAccess {
						.Type  = pass.get_depth_begin_access(),
						.Clear = {depth_clear},
					},
					.StencilBeginningAccess {
						.Type  = pass.get_stencil_begin_access(),
						.Clear = {stencil_clear},
					},
					.DepthEndingAccess {
						.Type = pass.get_depth_end_access(),
					},
					.StencilEndingAccess {
						.Type = pass.get_stencil_end_access(),
					},
				};
			}

			auto ds_desc_ptr = pass.uses_depth_stencil() ? &ds_desc : nullptr;
			cmd->BeginRenderPass(count(rt_descs), rt_descs.data(), ds_desc_ptr, D3D12_RENDER_PASS_FLAG_NONE);
			this->subpass_index = 1;
		}

		void transition_to_next_subpass()
		{
			VT_ASSERT(this->subpass_index < this->active_render_pass->subpass_count() - 1,
					  "All subpasses of this render pass have already been transitioned through.");

			auto& subpass_transitions = this->active_render_pass->get_subpass_transitions(this->subpass_index++);
			insert_render_pass_barriers(subpass_transitions);
		}

		void end_render_pass()
		{
			cmd->EndRenderPass();

			auto& final_transitions = this->active_render_pass->get_final_transitions();
			insert_render_pass_barriers(final_transitions);
		}

		void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets)
		{
			FixedList<D3D12_VERTEX_BUFFER_VIEW, MAX_VERTEX_ATTRIBUTES> views(first_buffer);

			auto offset = byte_offsets.begin() + first_buffer;
			for(auto& buffer : std::views::take(buffers, first_buffer))
				views.emplace_back(D3D12_VERTEX_BUFFER_VIEW {
					.BufferLocation = buffer.d3d12.get_gpu_address() + *offset++,
					.SizeInBytes	= buffer.get_size(),
					.StrideInBytes	= buffer.get_stride(),
				});

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

		void bind_primitive_topology(PrimitiveTopology topology, unsigned control_point_count = 0)
		{
			cmd->IASetPrimitiveTopology(convert_primitive_topology(topology, control_point_count));
		}

		void bind_viewports(ArrayView<Viewport> viewports)
		{
			static_assert(std::is_layout_compatible_v<Viewport, D3D12_VIEWPORT>);
			auto data = reinterpret_cast<D3D12_VIEWPORT const*>(viewports.data());

			cmd->RSSetViewports(count(viewports), data);
		}

		void bind_scissors(ArrayView<Rectangle> scissors)
		{
			FixedList<D3D12_RECT, MAX_ATTACHMENTS> rects;
			for(auto&& scissor : scissors)
				rects.emplace_back(D3D12_RECT {
					.left	= scissor.x,
					.top	= scissor.y,
					.right	= static_cast<LONG>(scissor.x + scissor.width),
					.bottom = static_cast<LONG>(scissor.y + scissor.height),
				});

			cmd->RSSetScissorRects(count(rects), rects.data());
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

	private:
		static constexpr D3D12_COMMAND_LIST_TYPE COMMAND_LIST_TYPE = convert_command_type(TYPE);

		D3D12Device*						  device;
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

		static D3D_PRIMITIVE_TOPOLOGY convert_primitive_topology(PrimitiveTopology topology, unsigned num_control_points)
		{
			using enum PrimitiveTopology;
			switch(topology)
			{ // clang-format off
				case PointList:		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
				case LineList:		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
				case LineStrip:		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
				case TriangleList:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				case TriangleStrip:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
				case PatchList:
					return static_cast<D3D_PRIMITIVE_TOPOLOGY>(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST - 1 +
															   num_control_points);
			} // clang-format on
			VT_UNREACHABLE();
		}

		void insert_render_pass_barriers(TransitionList const& transitions)
		{
			FixedList<D3D12_RESOURCE_BARRIER, MAX_ATTACHMENTS> barriers;
			for(auto transition : transitions)
			{
				barriers.emplace_back(D3D12_RESOURCE_BARRIER {
					.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
					.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
					.Transition {
						.pResource	 = this->active_render_target->get_attachment(transition.index),
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = transition.old_layout,
						.StateAfter	 = transition.new_layout,
					},
				});
			}
			cmd->ResourceBarrier(count(barriers), barriers.data());
		}
	};
}
