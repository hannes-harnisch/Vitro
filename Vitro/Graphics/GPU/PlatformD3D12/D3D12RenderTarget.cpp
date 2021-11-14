module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <array>
#include <ranges>
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
	using ResourcePointerArray = std::array<ID3D12Resource*, MAX_ATTACHMENTS>;

	// Contains a copy of/view into render target data for use in command lists, so we never have to store pointers to render
	// target, which is unsafe in case a render target gets moved. We don't need to guard against destruction, since that is
	// invalid no matter what and should be guarded against using a deferred deletion queue.
	export class CommandListRenderTargetData
	{
		friend class D3D12RenderTarget;

	public:
		CommandListRenderTargetData() = default;

		bool has_depth_stencil() const
		{
			return dsv.ptr != 0;
		}

		ID3D12Resource* get_attachment(size_t index) const
		{
			return resources[index];
		}

		// Returns nullptr if the render target contains no depth stencil attachment.
		ID3D12Resource* get_depth_stencil_attachment() const
		{
			return resources[attachment_count - 1];
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_render_target_view(size_t index) const
		{
			VT_ASSERT(index < attachment_count, "Index into render target descriptors out of range.");

			return {rtv_start.ptr + index * rtv_stride};
		}

		// Returns a null handle if the render target contains no depth stencil attachment.
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const
		{
			return dsv;
		}

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
									ResourcePointerArray const& resources) :
			rtv_start(rtv_start), dsv(dsv), attachment_count(attachment_count), rtv_stride(rtv_stride), resources(resources)
		{}
	};

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

		CommandListRenderTargetData get_data_for_command_list() const
		{
			return {
				descriptor_allocation.get(), get_dsv(), count_attachments(), get_pool().get_rtv_stride(), resources,
			};
		}

	private:
		// This struct is a functor acting as the slightly oversized deleter for the descriptor allocations done by this class.
		// This allows hacking std::unique_ptr a bit to actually account for two allocations at once, getting efficient move
		// semantics.
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

		DescriptorPool& get_pool() const
		{
			return *descriptor_allocation.get_deleter().pool;
		}

		unsigned count_attachments() const
		{
			return descriptor_allocation.get_deleter().total_attachments;
		}

		void set_pool_for_deleter(DescriptorPool& pool)
		{
			descriptor_allocation.get_deleter().pool = &pool;
		}

		// Increments the number of total attachments and returns the old value.
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

		D3D12_CPU_DESCRIPTOR_HANDLE get_dsv() const
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

			auto   range_start = resources.data() + dst_index;
			size_t byte_count  = sizeof resources[0] * (count_attachments() - dst_index);
			std::memmove(range_start + 1, range_start, byte_count);
			resources[dst_index] = back_buffer;
			increment();
		}

		void allocate_render_target_views(size_t units)
		{
			auto rtv_start = get_pool().allocate_render_target_views(units);
			descriptor_allocation.reset(rtv_start);
		}

		void create_render_target_views(ID3D12Device4& device)
		{
			auto const stride	 = get_pool().get_rtv_stride();
			auto	   rtv_start = descriptor_allocation.get();
			for(auto resource : resources | std::views::take(count_attachments()))
			{
				device.CreateRenderTargetView(resource, nullptr, rtv_start);
				rtv_start.ptr += stride;
			}
		}

		void maybe_allocate_depth_stencil_view(bool has_depth_stencil)
		{
			if(has_depth_stencil)
				get_dsv() = get_pool().allocate_depth_stencil_view();
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
