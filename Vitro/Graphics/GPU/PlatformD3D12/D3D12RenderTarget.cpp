module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <cstring>
#include <ranges>
module vt.Graphics.D3D12.RenderTarget;

import vt.Core.Ref;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.AssetResource;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;

namespace vt::d3d12
{
	bool CommandListRenderTargetData::has_depth_stencil() const
	{
		return dsv.ptr != 0;
	}

	ID3D12Resource* CommandListRenderTargetData::get_attachment(size_t index) const
	{
		return resources[index];
	}

	// Returns nullptr if the render target contains no depth stencil attachment.
	ID3D12Resource* CommandListRenderTargetData::get_depth_stencil_attachment() const
	{
		return resources[attachment_count - 1];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CommandListRenderTargetData::get_render_target_view(size_t index) const
	{
		VT_ASSERT(index < attachment_count, "Index into render target descriptors out of range.");

		return {rtv_start.ptr + index * rtv_stride};
	}

	// Returns a null handle if the render target contains no depth stencil attachment.
	D3D12_CPU_DESCRIPTOR_HANDLE CommandListRenderTargetData::get_depth_stencil_view() const
	{
		return dsv;
	}

	CommandListRenderTargetData::CommandListRenderTargetData(D3D12_CPU_DESCRIPTOR_HANDLE rtv_start,
															 D3D12_CPU_DESCRIPTOR_HANDLE dsv,
															 unsigned					 attachment_count,
															 unsigned					 rtv_stride,
															 ResourcePointerArray const& in_resources) :
		rtv_start(rtv_start), dsv(dsv), attachment_count(attachment_count), rtv_stride(rtv_stride)
	{
		std::memcpy(resources, in_resources, sizeof resources);
	}

	D3D12RenderTarget::D3D12RenderTarget(RenderTargetSpecification const& spec,
										 ID3D12Device4&					  device,
										 DescriptorPool&				  descriptor_pool)
	{
		set_pool_for_deleter(descriptor_pool);
		initialize_resource_ptrs(spec.color_attachments);

		allocate_render_target_views(spec.color_attachments.size());
		create_render_target_views(device);

		maybe_allocate_depth_stencil_view(spec.depth_stencil_attachment);
		maybe_create_depth_stencil_view(device, spec.depth_stencil_attachment);
	}

	D3D12RenderTarget::D3D12RenderTarget(SharedRenderTargetSpecification const& spec,
										 SwapChain const&						swap_chain,
										 unsigned								back_buffer_index,
										 ID3D12Device4&							device,
										 DescriptorPool&						descriptor_pool)
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
	void D3D12RenderTarget::recreate(ID3D12Device4& device, RenderTargetSpecification const& spec)
	{
		reset_attachment_count();
		initialize_resource_ptrs(spec.color_attachments);

		create_render_target_views(device);
		maybe_create_depth_stencil_view(device, spec.depth_stencil_attachment);
	}

	// The existing allocation will be reused here instead of constructing a completely new render target.
	void D3D12RenderTarget::recreate(ID3D12Device4&							device,
									 SharedRenderTargetSpecification const& spec,
									 SwapChain const&						swap_chain,
									 unsigned								back_buffer_index)
	{
		reset_attachment_count();
		initialize_resource_ptrs(spec.color_attachments);

		emplace_swap_chain_resource(swap_chain, back_buffer_index, spec.shared_img_dst_index);

		create_render_target_views(device);
		maybe_create_depth_stencil_view(device, spec.depth_stencil_attachment);
	}

	CommandListRenderTargetData D3D12RenderTarget::get_data_for_command_list() const
	{
		return {
			descriptor_allocation.get(), get_dsv(), count_attachments(), get_pool().get_rtv_stride(), resources,
		};
	}

	DescriptorPool& D3D12RenderTarget::get_pool() const
	{
		return *descriptor_allocation.get_deleter().pool;
	}

	unsigned D3D12RenderTarget::count_attachments() const
	{
		return descriptor_allocation.get_deleter().total_attachments;
	}

	void D3D12RenderTarget::set_pool_for_deleter(DescriptorPool& pool)
	{
		descriptor_allocation.get_deleter().pool = &pool;
	}

	// Increments the number of total attachments and returns the old value.
	unsigned D3D12RenderTarget::increment()
	{
		return descriptor_allocation.get_deleter().total_attachments++;
	}

	void D3D12RenderTarget::reset_attachment_count()
	{
		descriptor_allocation.get_deleter().total_attachments = 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE& D3D12RenderTarget::get_dsv()
	{
		return descriptor_allocation.get_deleter().depth_stencil_view;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderTarget::get_dsv() const
	{
		return descriptor_allocation.get_deleter().depth_stencil_view;
	}

	void D3D12RenderTarget::initialize_resource_ptrs(ConstSpan<CRef<Image>> color_attachments)
	{
		for(Image const& attachment : color_attachments)
			resources[increment()] = attachment.d3d12.get_resource();
	}

	void D3D12RenderTarget::emplace_swap_chain_resource(SwapChain const& swap_chain,
														unsigned		 back_buffer_index,
														unsigned		 dst_index)
	{
		auto back_buffer = swap_chain.d3d12.get_back_buffer_ptr(back_buffer_index);

		auto   range_start = resources + dst_index;
		size_t byte_count  = sizeof resources[0] * (count_attachments() - dst_index);
		std::memmove(range_start + 1, range_start, byte_count);
		resources[dst_index] = back_buffer;
		increment();
	}

	void D3D12RenderTarget::allocate_render_target_views(size_t units)
	{
		auto rtv_start = get_pool().allocate_render_target_views(units);
		descriptor_allocation.reset(rtv_start);
	}

	void D3D12RenderTarget::create_render_target_views(ID3D12Device4& device)
	{
		auto const stride	 = get_pool().get_rtv_stride();
		auto	   rtv_start = descriptor_allocation.get();
		for(auto resource : resources | std::views::take(count_attachments()))
		{
			device.CreateRenderTargetView(resource, nullptr, rtv_start);
			rtv_start.ptr += stride;
		}
	}

	void D3D12RenderTarget::maybe_allocate_depth_stencil_view(bool has_depth_stencil)
	{
		if(has_depth_stencil)
			get_dsv() = get_pool().allocate_depth_stencil_view();
		else
			get_dsv() = {};
	}

	void D3D12RenderTarget::maybe_create_depth_stencil_view(ID3D12Device4& device, Image const* depth_stencil_attachment)
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
}
