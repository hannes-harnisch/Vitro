module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.CommandList;

import vt.Core.FixedList;
import vt.Core.Rect;
import vt.Core.SmallList;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.RenderPass;
import vt.Graphics.D3D12.RenderTarget;
import vt.Graphics.D3D12.RootSignature;
import vt.Graphics.AssetResource;
import vt.Graphics.AbstractCommandList;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Handle;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;

namespace vt::d3d12
{
	template<CommandType> class CommandListData;

	template<> class CommandListData<CommandType::Copy>
	{};

	template<> class CommandListData<CommandType::Compute> : protected CommandListData<CommandType::Copy>
	{
	protected:
		DescriptorPool*			descriptor_pool;
		RootParameterMap		bound_compute_root_indices;
		ID3D12CommandSignature* dispatch_signature;
	};

	template<> class CommandListData<CommandType::Render> : protected CommandListData<CommandType::Compute>
	{
	protected:
		ID3D12CommandSignature*				   draw_signature;
		ID3D12CommandSignature*				   draw_indexed_signature;
		RootParameterMap					   bound_render_root_indices;
		CommandListRenderPassData			   bound_render_pass;
		CommandListRenderTargetData			   bound_render_target;
		D3D_PRIMITIVE_TOPOLOGY				   bound_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		unsigned							   subpass_index;
		FixedList<ClearValue, MAX_ATTACHMENTS> clear_values;
	};

	export template<CommandType TYPE>
	class D3D12CommandList final : public AbstractCommandList<TYPE>, private CommandListData<TYPE>
	{
	public:
		D3D12CommandList(ID3D12Device4&			 device,
						 DescriptorPool&		 descriptor_pool,
						 ID3D12CommandSignature* dispatch_signature,
						 ID3D12CommandSignature* draw_signature,
						 ID3D12CommandSignature* draw_indexed_signature);

		CommandListHandle get_handle();

		void reset();
		void begin();
		void end();
		void copy_buffer(Buffer const& src, Buffer& dst);
		void copy_image(Image const& src, Image& dst);
		void copy_buffer_to_image(Buffer const& src, Image& dst);
		void update_buffer(Buffer& dst, size_t offset, size_t size, void const* data);
		void copy_buffer_region(Buffer const& src, Buffer& dst, size_t src_offset, size_t dst_offset, size_t size);
		void copy_image_region(Image const& src, Image& dst, ImageCopyRegion const& region);
		void bind_compute_pipeline(ComputePipeline const& pipeline);
		void bind_compute_root_signature(RootSignature const& root_signature);
		void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets);
		void push_compute_constants(size_t byte_offset, size_t byte_size, void const* data);
		void dispatch(unsigned x_count, unsigned y_count, unsigned z_count);
		void dispatch_indirect(Buffer const& buffer, size_t offset);
		void begin_render_pass(RenderPass const&	 render_pass,
							   RenderTarget const&	 render_target,
							   ConstSpan<ClearValue> clear_values = {});
		void change_subpass();
		void end_render_pass();
		void bind_render_pipeline(RenderPipeline const& pipeline);
		void bind_render_root_signature(RootSignature const& root_signature);
		void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets);
		void push_render_constants(size_t byte_offset, size_t byte_size, void const* data);
		void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets);
		void bind_index_buffer(Buffer const& buffer, size_t byte_offset);
		void set_viewports(ArrayView<Viewport> viewports);
		void set_scissors(ArrayView<Rectangle> scissors);
		void set_blend_constants(Float4 blend_constants);
		void set_depth_bounds(float min, float max);
		void set_stencil_reference(unsigned reference_value);
		void draw(unsigned vertex_count, unsigned instance_count, unsigned first_vertex, unsigned first_instance);
		void draw_indexed(unsigned index_count,
						  unsigned instance_count,
						  unsigned first_index,
						  int	   vertex_offset,
						  unsigned first_instance);
		void draw_indirect(Buffer const& buffer, size_t offset, unsigned draws);
		void draw_indexed_indirect(Buffer const& buffer, size_t offset, unsigned draws);

	private:
		ComUnique<ID3D12CommandAllocator>	  allocator;
		ComUnique<ID3D12GraphicsCommandList4> cmd;

		void				insert_subpass_barriers(TransitionList const& transitions) const;
		void				begin_subpass(unsigned subpass_index) const;
		template<bool> void bind_descriptor_sets(ArrayView<DescriptorSet> sets, RootParameterMap const& indices);
		template<bool> void bind_descriptor_table(UINT index, D3D12_GPU_DESCRIPTOR_HANDLE table);
		template<bool> void bind_cbv(UINT index, D3D12_GPU_VIRTUAL_ADDRESS view);
		template<bool> void bind_srv(UINT index, D3D12_GPU_VIRTUAL_ADDRESS view);
		template<bool> void bind_uav(UINT index, D3D12_GPU_VIRTUAL_ADDRESS view);
	};
}
