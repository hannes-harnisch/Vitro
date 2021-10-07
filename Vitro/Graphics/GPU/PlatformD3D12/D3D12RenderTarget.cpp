module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <type_traits>
#include <utility>
export module vt.Graphics.D3D12.RenderTarget;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.SwapPtr;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.AssetResource;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;

namespace vt::d3d12
{
	export class D3D12RenderTarget
	{
	public:
		D3D12RenderTarget(DescriptorPool& descriptor_pool, RenderTargetSpecification const& spec) : pool(&descriptor_pool)
		{
			initialize_resource_ptrs(spec.color_attachments);
			allocate_render_target_views(spec.color_attachments.size());
			create_render_target_views();
			maybe_allocate_depth_stencil_view(spec.depth_stencil_attachment);
			maybe_create_depth_stencil_view(spec.depth_stencil_attachment);
		}

		D3D12RenderTarget(DescriptorPool&						 descriptor_pool,
						  SharedRenderTargetSpecification const& spec,
						  SwapChain const&						 swap_chain,
						  unsigned								 back_buffer_index) :
			pool(&descriptor_pool)
		{
			initialize_resource_ptrs(spec.color_attachments);
			emplace_swap_chain_resource(swap_chain, back_buffer_index, spec.shared_img_dst_index);
			allocate_render_target_views(spec.color_attachments.size() + 1);
			create_render_target_views();
			maybe_allocate_depth_stencil_view(spec.depth_stencil_attachment);
			maybe_create_depth_stencil_view(spec.depth_stencil_attachment);
		}

		~D3D12RenderTarget()
		{
			pool->deallocate_render_target_views({rtv_begin.get(), count(resources) - 1});
			pool->deallocate_depth_stencil_views({dsv.get(), 1});
		}

		void recreate(RenderTargetSpecification const& spec)
		{
			resources.clear();
			initialize_resource_ptrs(spec.color_attachments);
			create_render_target_views();
			maybe_create_depth_stencil_view(spec.depth_stencil_attachment);
		}

		void recreate(SharedRenderTargetSpecification const& spec, SwapChain const& swap_chain, unsigned back_buffer_index)
		{
			resources.clear();
			initialize_resource_ptrs(spec.color_attachments);
			emplace_swap_chain_resource(swap_chain, back_buffer_index, spec.shared_img_dst_index);
			create_render_target_views();
			maybe_create_depth_stencil_view(spec.depth_stencil_attachment);
		}

		bool has_depth_stencil() const
		{
			return dsv.get().ptr != 0;
		}

		ID3D12Resource* get_attachment(size_t index) const
		{
			return resources[index];
		}

		// Returns nullptr if the render target contains no depth stencil attachment.
		ID3D12Resource* get_depth_stencil_attachment() const
		{
			return resources.back();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_view(size_t index) const
		{
			VT_ASSERT(index <= resources.size(), "Index into render target descriptors out of range.");

			if(index == resources.size())
				return dsv.get();
			else
				return {rtv_begin.get().ptr + index * pool->get_rtv_increment()};
		}

		// Returns a null handle if the render target contains no depth stencil attachment.
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const
		{
			return dsv.get();
		}

		D3D12RenderTarget(D3D12RenderTarget&&) = default;
		D3D12RenderTarget& operator=(D3D12RenderTarget&&) = default;

	private:
		DescriptorPool*								pool;
		FixedList<ID3D12Resource*, MAX_ATTACHMENTS> resources;
		SwapPtr<D3D12_CPU_DESCRIPTOR_HANDLE>		rtv_begin;
		SwapPtr<D3D12_CPU_DESCRIPTOR_HANDLE>		dsv;

		void initialize_resource_ptrs(ConstSpan<Texture const*> color_attachments)
		{
			for(auto attachment : color_attachments)
				resources.emplace_back(attachment->d3d12.ptr());
		}

		void emplace_swap_chain_resource(SwapChain const& swap_chain, unsigned back_buffer_index, unsigned dst_index)
		{
			auto back_buffer = swap_chain.d3d12.get_back_buffer_ptr(back_buffer_index);
			resources.emplace(resources.begin() + dst_index, back_buffer);
		}

		void allocate_render_target_views(size_t units)
		{
			auto rtv_alloc = pool->allocate_render_target_views(units);
			rtv_begin.set(rtv_alloc.handle);
		}

		void create_render_target_views()
		{
			auto const increment = pool->get_rtv_increment();
			auto	   temp_rtv	 = rtv_begin.get();
			for(auto resource : resources)
			{
				pool->get_device()->CreateRenderTargetView(resource, nullptr, temp_rtv);
				temp_rtv.ptr += increment;
			}
		}

		void maybe_allocate_depth_stencil_view(bool has_depth_stencil)
		{
			if(has_depth_stencil)
			{
				auto dsv_alloc = pool->allocate_depth_stencil_view();
				dsv.set(dsv_alloc.handle);
			}
			else
				dsv = {};
		}

		void maybe_create_depth_stencil_view(Texture const* depth_stencil_attachment)
		{
			if(depth_stencil_attachment)
			{
				auto ds_resource = depth_stencil_attachment->d3d12.ptr();
				resources.emplace_back(ds_resource);
				pool->get_device()->CreateDepthStencilView(ds_resource, nullptr, dsv.get());
			}
			else
				resources.emplace_back(nullptr); // Insert nullptr so that asking for a depth stencil from outside returns null
		}
	};

	static_assert(std::is_move_constructible_v<D3D12RenderTarget>);
}
