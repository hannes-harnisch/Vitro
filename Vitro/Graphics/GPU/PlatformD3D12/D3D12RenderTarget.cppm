module;
#include "D3D12API.hpp"

#include <memory>
export module vt.Graphics.D3D12.RenderTarget;

import vt.Core.Ref;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.AssetResource;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;

namespace vt::d3d12
{
	using ResourcePointerArray = ID3D12Resource* [MAX_ATTACHMENTS];

	// Contains a copy of/view into render target data for use in command lists, so we never have to store pointers to render
	// target, which is unsafe in case a render target gets moved. We don't need to guard against destruction, since that is
	// invalid no matter what and should be guarded against using a deferred deletion queue.
	export class CommandListRenderTargetData
	{
		friend class D3D12RenderTarget;

	public:
		CommandListRenderTargetData() = default;

		bool			has_depth_stencil() const;
		ID3D12Resource* get_attachment(size_t index) const;

		// Returns nullptr if the render target contains no depth stencil attachment.
		ID3D12Resource*				get_depth_stencil_attachment() const;
		D3D12_CPU_DESCRIPTOR_HANDLE get_render_target_view(size_t index) const;
		// Returns a null handle if the render target contains no depth stencil attachment.
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const;

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_start;
		D3D12_CPU_DESCRIPTOR_HANDLE dsv;
		unsigned					attachment_count;
		unsigned					rtv_stride;
		ResourcePointerArray		resources;

		CommandListRenderTargetData(D3D12_CPU_DESCRIPTOR_HANDLE rtv_start,
									D3D12_CPU_DESCRIPTOR_HANDLE dsv,
									unsigned					attachment_count,
									unsigned					rtv_stride,
									ResourcePointerArray const& resources);
	};

	export class D3D12RenderTarget
	{
	public:
		D3D12RenderTarget(RenderTargetSpecification const& spec, ID3D12Device4& device, DescriptorPool& descriptor_pool);
		D3D12RenderTarget(SharedRenderTargetSpecification const& spec,
						  SwapChain const&						 swap_chain,
						  unsigned								 back_buffer_index,
						  ID3D12Device4&						 device,
						  DescriptorPool&						 descriptor_pool);

		// The existing allocation will be reused here instead of constructing a completely new render target.
		void recreate(ID3D12Device4& device, RenderTargetSpecification const& spec);

		// The existing allocation will be reused here instead of constructing a completely new render target.
		void recreate(ID3D12Device4&						 device,
					  SharedRenderTargetSpecification const& spec,
					  SwapChain const&						 swap_chain,
					  unsigned								 back_buffer_index);

		CommandListRenderTargetData get_data_for_command_list() const;

	private:
		// This struct is a functor acting as the slightly oversized deleter for the descriptor allocations done by this
		// class. This allows hacking std::unique_ptr a bit to actually account for two allocations at once, getting
		// efficient move semantics.
		struct DescriptorDeallocator
		{
			using pointer = PointerProxy<D3D12_CPU_DESCRIPTOR_HANDLE>;

			DescriptorPool*				pool;
			D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_view = {};
			unsigned					total_attachments  = 0;

			void operator()(D3D12_CPU_DESCRIPTOR_HANDLE rtv_begin) const
			{
				pool->deallocate_render_target_views(rtv_begin, total_attachments - 1);
				pool->deallocate_depth_stencil_view(depth_stencil_view);
			}
		};
		using UniqueDescriptorAllocation = std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE, DescriptorDeallocator>;

		UniqueDescriptorAllocation descriptor_allocation;
		ResourcePointerArray	   resources {};

		DescriptorPool& get_pool() const;
		unsigned		count_attachments() const;
		void			set_pool_for_deleter(DescriptorPool& pool);

		// Increments the number of total attachments and returns the old value.
		unsigned increment();

		void						 reset_attachment_count();
		D3D12_CPU_DESCRIPTOR_HANDLE& get_dsv();
		D3D12_CPU_DESCRIPTOR_HANDLE	 get_dsv() const;
		void						 initialize_resource_ptrs(ConstSpan<CRef<Image>> color_attachments);
		void emplace_swap_chain_resource(SwapChain const& swap_chain, unsigned back_buffer_index, unsigned dst_index);
		void allocate_render_target_views(size_t units);
		void create_render_target_views(ID3D12Device4& device);
		void maybe_allocate_depth_stencil_view(bool has_depth_stencil);
		void maybe_create_depth_stencil_view(ID3D12Device4& device, Image const* depth_stencil_attachment);
	};
}
