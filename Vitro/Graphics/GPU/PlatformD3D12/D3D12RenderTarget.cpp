module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <ranges>
export module vt.Graphics.D3D12.RenderTarget;

import vt.Core.Ref;
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
		D3D12RenderTarget(RenderTargetSpecification const& spec, ID3D12Device4& device, DescriptorPool& descriptor_pool)
		{
			set_pool_for_deleter(descriptor_pool);
			initialize_resource_ptrs(spec.color_attachments);

			allocate_render_target_views(spec.color_attachments.size());
			create_render_target_views(device);

			maybe_allocate_depth_stencil_view(spec.depth_stencil_attachment);
			maybe_create_depth_stencil_view(device, spec.depth_stencil_attachment);
		}

		D3D12RenderTarget(SharedRenderTargetSpecification const& spec,
						  SwapChain const&						 swap_chain,
						  unsigned								 back_buffer_index,
						  ID3D12Device4&						 device,
						  DescriptorPool&						 descriptor_pool)
		{
			set_pool_for_deleter(descriptor_pool);
			initialize_resource_ptrs(spec.color_attachments);
			emplace_swap_chain_resource(swap_chain, back_buffer_index, spec.shared_img_dst_index);

			allocate_render_target_views(spec.color_attachments.size() + 1);
			create_render_target_views(device);

			maybe_allocate_depth_stencil_view(spec.depth_stencil_attachment);
			maybe_create_depth_stencil_view(device, spec.depth_stencil_attachment);
		}

		// The existing allocation will be reused here instead of constructing a completely new render target.
		void recreate(ID3D12Device4& device, RenderTargetSpecification const& spec)
		{
			reset_attachment_count();
			initialize_resource_ptrs(spec.color_attachments);

			create_render_target_views(device);
			maybe_create_depth_stencil_view(device, spec.depth_stencil_attachment);
		}

		// The existing allocation will be reused here instead of constructing a completely new render target.
		void recreate(ID3D12Device4&						 device,
					  SharedRenderTargetSpecification const& spec,
					  SwapChain const&						 swap_chain,
					  unsigned								 back_buffer_index)
		{
			reset_attachment_count();
			initialize_resource_ptrs(spec.color_attachments);

			emplace_swap_chain_resource(swap_chain, back_buffer_index, spec.shared_img_dst_index);

			create_render_target_views(device);
			maybe_create_depth_stencil_view(device, spec.depth_stencil_attachment);
		}

		bool has_depth_stencil() const
		{
			return get_depth_stencil_view().ptr != 0;
		}

		ID3D12Resource* get_attachment(size_t index) const
		{
			return resources[index];
		}

		// Returns nullptr if the render target contains no depth stencil attachment.
		ID3D12Resource* get_depth_stencil_attachment() const
		{
			return resources[get_count() - 1];
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_render_target_view(size_t index) const
		{
			VT_ASSERT(index < get_count(), "Index into render target descriptors out of range.");
			return {get_rtv_begin() + index * get_pool().get_rtv_stride()};
		}

		// Returns a null handle if the render target contains no depth stencil attachment.
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const
		{
			return descriptor_allocation.get_deleter().depth_stencil_view;
		}

	private:
		// This struct is a very big functor acting as the deleter for the descriptor allocations done by this class. This
		// allows hacking std::unique_ptr a bit to actually account for two allocations at once, getting efficient move
		// semantics.
		struct DescriptorDeallocator
		{
			DescriptorPool*				pool;
			D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_view = {};
			unsigned					total_attachments  = 0;

			void operator()(void* descriptor_handle_value) const
			{
				D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle {reinterpret_cast<SIZE_T>(descriptor_handle_value)};
				pool->deallocate_render_target_views({rtv_handle, total_attachments - 1});
				pool->deallocate_depth_stencil_views({depth_stencil_view, 1});
			}
		};
		// We have to use void* as the unique pointer type, because it has to be assignable with a nullptr literal.
		using UniqueDescriptorAllocation = std::unique_ptr<void, DescriptorDeallocator>;

		UniqueDescriptorAllocation descriptor_allocation;
		ID3D12Resource*			   resources[MAX_ATTACHMENTS];

		SIZE_T get_rtv_begin() const
		{
			return reinterpret_cast<SIZE_T>(descriptor_allocation.get());
		}

		DescriptorPool& get_pool() const
		{
			return *descriptor_allocation.get_deleter().pool;
		}

		unsigned get_count() const
		{
			return descriptor_allocation.get_deleter().total_attachments;
		}

		void set_pool_for_deleter(DescriptorPool& pool)
		{
			descriptor_allocation.get_deleter().pool = &pool;
		}

		unsigned increment()
		{
			return descriptor_allocation.get_deleter().total_attachments++;
		}

		void reset_attachment_count()
		{
			descriptor_allocation.get_deleter().total_attachments = 0;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE& get_dsv()
		{
			return descriptor_allocation.get_deleter().depth_stencil_view;
		}

		void initialize_resource_ptrs(ConstSpan<CRef<Image>> color_attachments)
		{
			for(Image const& attachment : color_attachments)
				resources[increment()] = attachment.d3d12.get_resource();
		}

		void emplace_swap_chain_resource(SwapChain const& swap_chain, unsigned back_buffer_index, unsigned dst_index)
		{
			auto back_buffer = swap_chain.d3d12.get_back_buffer_ptr(back_buffer_index);

			auto range_start = resources + dst_index;
			std::memmove(range_start + 1, range_start, (get_count() - dst_index) * sizeof(*resources));
			resources[dst_index] = back_buffer;
			increment();
		}

		void allocate_render_target_views(size_t units)
		{
			auto rtv_alloc = get_pool().allocate_render_target_views(units);

			auto opaque_ptr = reinterpret_cast<void*>(rtv_alloc.handle.ptr);
			descriptor_allocation.reset(opaque_ptr);
		}

		void create_render_target_views(ID3D12Device4& device)
		{
			auto const stride	= get_pool().get_rtv_stride();
			auto	   temp_rtv = get_rtv_begin();
			for(auto resource : resources | std::views::take(get_count()))
			{
				device.CreateRenderTargetView(resource, nullptr, {temp_rtv});
				temp_rtv += stride;
			}
		}

		void maybe_allocate_depth_stencil_view(bool has_depth_stencil)
		{
			if(has_depth_stencil)
			{
				auto dsv_alloc = get_pool().allocate_depth_stencil_view();
				get_dsv()	   = dsv_alloc.handle;
			}
			else
				get_dsv() = {};
		}

		void maybe_create_depth_stencil_view(ID3D12Device4& device, Image const* depth_stencil_attachment)
		{
			if(depth_stencil_attachment)
			{
				auto depth_stencil_img = depth_stencil_attachment->d3d12.get_resource();
				resources[increment()] = depth_stencil_img;

				device.CreateDepthStencilView(depth_stencil_img, nullptr, get_dsv());
			}
			else
				resources[increment()] = nullptr; // Insert nullptr so that asking for the DSV from outside returns null.
		}
	};
}
