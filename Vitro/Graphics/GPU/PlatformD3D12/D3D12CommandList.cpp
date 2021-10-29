module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <cstdlib>
#include <ranges>
#include <type_traits>
export module vt.Graphics.D3D12.CommandList;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Rectangle;
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
		RootSignatureParameterMap const* bound_compute_root_indices = nullptr;
		ID3D12CommandSignature*			 dispatch_signature;
	};

	template<> class CommandListData<CommandType::Render> : protected CommandListData<CommandType::Compute>
	{
	protected:
		ID3D12CommandSignature*			 draw_signature;
		ID3D12CommandSignature*			 draw_indexed_signature;
		RootSignatureParameterMap const* bound_render_root_indices = nullptr;
		D3D12RenderPass const*			 bound_render_pass		   = nullptr;
		D3D12RenderTarget const*		 bound_render_target	   = nullptr;
		unsigned						 subpass_index			   = 1;
		D3D_PRIMITIVE_TOPOLOGY			 bound_primitive_topology  = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	};

	export template<CommandType TYPE> class D3D12CommandList final : public CommandListBase<TYPE>, private CommandListData<TYPE>
	{
	public:
		D3D12CommandList(ID3D12Device4*			 device,
						 DescriptorPool&		 pool,
						 ID3D12CommandSignature* dispatch_signature,
						 ID3D12CommandSignature* draw_signature,
						 ID3D12CommandSignature* draw_indexed_signature) :
			descriptor_pool(&pool)
		{
			if constexpr(TYPE != CommandType::Copy)
				this->dispatch_signature = dispatch_signature;
			if constexpr(TYPE == CommandType::Render)
			{
				this->draw_signature		 = draw_signature;
				this->draw_indexed_signature = draw_indexed_signature;
			}

			auto result = device->CreateCommandAllocator(COMMAND_LIST_TYPE, VT_COM_OUT(allocator));
			VT_CHECK_RESULT(result, "Failed to create D3D12 command allocator.");

			result = device->CreateCommandList1(0, COMMAND_LIST_TYPE, D3D12_COMMAND_LIST_FLAG_NONE, VT_COM_OUT(cmd));
			VT_CHECK_RESULT(result, "Failed to create D3D12 command list.");
		}

		CommandListHandle handle()
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

			auto heaps = descriptor_pool->get_shader_visible_heaps();
			cmd->SetDescriptorHeaps(2, heaps.data());
		}

		void end()
		{
			auto result = cmd->Close();
			VT_CHECK_RESULT(result, "Failed to end D3D12 command list.");

			if constexpr(TYPE == CommandType::Render)
				this->bound_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}

		void bind_compute_pipeline(ComputePipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.ptr());
		}

		void bind_compute_root_signature(RootSignature const& root_signature)
		{
			cmd->SetComputeRootSignature(root_signature.d3d12.ptr());
			this->bound_compute_root_indices = &root_signature.d3d12.get_parameter_map();
		}

		void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			for(auto& set : descriptor_sets)
			{
				UINT index = this->bound_compute_root_indices->find(set.d3d12.get_layout_id());
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

		void dispatch_indirect(Buffer const& buffer, size_t offset)
		{
			cmd->ExecuteIndirect(this->dispatch_signature, 1, buffer.d3d12.get_resource(), offset, nullptr, 0);
		}

		void begin_render_pass(RenderPass const&	 render_pass,
							   RenderTarget const&	 render_target,
							   ConstSpan<ClearValue> clear_values = {})
		{
			this->bound_render_pass	  = &render_pass.d3d12;
			this->bound_render_target = &render_target.d3d12;

			auto& pass	 = *this->bound_render_pass;
			auto& target = *this->bound_render_target;

			auto& subpass = pass.get_subpass(0);
			insert_render_pass_barriers(subpass.transitions);

			FixedList<D3D12_RENDER_PASS_RENDER_TARGET_DESC, MAX_COLOR_ATTACHMENTS> rt_descs;

			size_t index = 0;
			for(auto access : pass.get_render_target_accesses())
			{
				D3D12_CLEAR_VALUE color_clear;
				if(clear_values.empty())
					color_clear = {};
				else
					color_clear = {
						.Format = access.format,
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
						.Type  = access.begin_access,
						.Clear = {color_clear},
					},
					.EndingAccess {
						.Type = access.end_access,
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
			cmd->BeginRenderPass(count(rt_descs), rt_descs.data(), ds_desc_ptr, subpass.flags);
			this->subpass_index = 1;
		}

		void transition_subpass()
		{
			auto& pass = *this->bound_render_pass;
			VT_ASSERT(this->subpass_index < pass.subpass_count() - 1,
					  "All subpasses of this render pass have already been transitioned through.");

			cmd->EndRenderPass();

			auto& subpass = pass.get_subpass(this->subpass_index);
			insert_render_pass_barriers(subpass.transitions);

			FixedList<D3D12_RENDER_PASS_RENDER_TARGET_DESC, MAX_COLOR_ATTACHMENTS> rt_descs;
			// TODO: finish logic on this
			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC ds_desc;

			auto ds_desc_ptr = pass.uses_depth_stencil() ? &ds_desc : nullptr;
			cmd->BeginRenderPass(count(rt_descs), rt_descs.data(), ds_desc_ptr, subpass.flags);

			this->subpass_index++;
			throw "Not implemented.";
		}

		void end_render_pass()
		{
			cmd->EndRenderPass();

			auto& final_transitions = this->bound_render_pass->get_final_transitions();
			insert_render_pass_barriers(final_transitions);
		}

		void bind_render_pipeline(RenderPipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.ptr());

			auto pipeline_topology = pipeline.d3d12.get_topology();
			if(this->bound_primitive_topology != pipeline_topology)
			{
				cmd->IASetPrimitiveTopology(pipeline_topology);
				this->bound_primitive_topology = pipeline_topology;
			}
		}

		void bind_render_root_signature(RootSignature const& root_signature)
		{
			cmd->SetGraphicsRootSignature(root_signature.d3d12.ptr());
			this->bound_render_root_indices = &root_signature.d3d12.get_parameter_map();
		}

		void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets)
		{
			for(auto& set : descriptor_sets)
			{
				UINT index = this->bound_render_root_indices->find(set.d3d12.get_layout_id());
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

		void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets)
		{
			FixedList<D3D12_VERTEX_BUFFER_VIEW, MAX_VERTEX_BUFFERS> views(first_buffer);

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

		void set_viewports(ArrayView<Viewport> viewports)
		{
			static_assert(std::is_layout_compatible_v<Viewport, D3D12_VIEWPORT>);

			auto data = reinterpret_cast<D3D12_VIEWPORT const*>(viewports.data());
			cmd->RSSetViewports(count(viewports), data);
		}

		void set_scissors(ArrayView<Rectangle> scissors)
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

		void set_blend_constants(Float4 blend_constants)
		{
			float const factor[] {
				blend_constants.r,
				blend_constants.g,
				blend_constants.b,
				blend_constants.a,
			};
			cmd->OMSetBlendFactor(factor);
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
		static constexpr D3D12_COMMAND_LIST_TYPE COMMAND_LIST_TYPE = convert_command_type(TYPE);

		DescriptorPool*						  descriptor_pool;
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

		void insert_render_pass_barriers(TransitionList const& transitions)
		{
			FixedList<D3D12_RESOURCE_BARRIER, MAX_ATTACHMENTS> barriers;
			for(auto transition : transitions)
			{
				barriers.emplace_back(D3D12_RESOURCE_BARRIER {
					.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
					.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
					.Transition {
						.pResource	 = this->bound_render_target->get_attachment(transition.index),
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
