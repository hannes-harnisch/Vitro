module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <utility>
export module vt.Graphics.D3D12.RenderTarget;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.AssetResource;
import vt.Graphics.RenderTargetSpecification;

namespace vt::d3d12
{
	export class D3D12RenderTarget
	{
	public:
		D3D12RenderTarget(DescriptorPool& descriptor_pool, RenderTargetSpecification const& spec) : pool(&descriptor_pool)
		{
			size_t view_count = initialize_color_attachment_resources(spec);

			auto rtv_alloc = pool->allocate_render_target_views(view_count);
			rtv_begin	   = rtv_alloc.handle;
			initialize_render_target_views();

			if(spec.depth_stencil_attachment)
			{
				auto dsv_alloc = pool->allocate_depth_stencil_view();
				dsv			   = dsv_alloc.handle;

				auto ds_resource = spec.depth_stencil_attachment->d3d12.ptr();
				resources.emplace_back(ds_resource);
				pool->get_device()->CreateDepthStencilView(ds_resource, nullptr, dsv);
			}
			else
			{
				dsv = {};
				resources.emplace_back(nullptr); // Insert nullptr so that asking for a depth stencil from outside returns null
			}
		}

		D3D12RenderTarget(D3D12RenderTarget&& that) noexcept :
			pool(that.pool),
			resources(std::move(that.resources)),
			rtv_begin(std::exchange(that.rtv_begin, {})),
			dsv(std::exchange(that.dsv, {}))
		{}

		~D3D12RenderTarget()
		{
			pool->deallocate_render_target_views({rtv_begin, count(resources) - 1});
			pool->deallocate_depth_stencil_views({dsv, 1});
		}

		D3D12RenderTarget& operator=(D3D12RenderTarget&& that) noexcept
		{
			pool	  = that.pool;
			resources = std::move(that.resources);
			std::swap(rtv_begin, that.rtv_begin);
			std::swap(dsv, that.dsv);
			return *this;
		}

		void recreate(RenderTargetSpecification const& spec)
		{
			resources.clear();
			initialize_color_attachment_resources(spec);
			initialize_render_target_views();

			if(spec.depth_stencil_attachment)
			{
				auto ds_resource = spec.depth_stencil_attachment->d3d12.ptr();
				resources.emplace_back(ds_resource);
				pool->get_device()->CreateDepthStencilView(ds_resource, nullptr, dsv);
			}
			else
				resources.emplace_back(nullptr);
		}

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
			return resources.back();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_view(size_t index) const
		{
			VT_ASSERT(index <= resources.size(), "Index into render target descriptors out of range.");

			if(index == resources.size())
				return dsv;
			else
				return {rtv_begin.ptr + index * pool->get_rtv_increment()};
		}

		// Returns a null handle if the render target contains no depth stencil attachment.
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const
		{
			return dsv;
		}

	private:
		DescriptorPool*								pool;
		FixedList<ID3D12Resource*, MAX_ATTACHMENTS> resources;
		D3D12_CPU_DESCRIPTOR_HANDLE					rtv_begin;
		D3D12_CPU_DESCRIPTOR_HANDLE					dsv;

		size_t initialize_color_attachment_resources(RenderTargetSpecification const& spec)
		{
			int	   extra	  = spec.swap_chain ? 1 : 0;
			size_t view_count = spec.color_attachments.size() + extra;

			for(auto attachment : spec.color_attachments)
				resources.emplace_back(attachment->d3d12.ptr());

			if(spec.swap_chain)
			{
				auto back_buffer = spec.swap_chain->d3d12.get_back_buffer_ptr(spec.swap_chain_src_index);
				resources.emplace(resources.begin() + spec.swap_chain_dst_index, back_buffer);
			}

			return view_count;
		}

		void initialize_render_target_views()
		{
			auto const increment = pool->get_rtv_increment();
			auto	   temp_rtv	 = rtv_begin;
			for(auto resource : resources)
			{
				pool->get_device()->CreateRenderTargetView(resource, nullptr, temp_rtv);
				temp_rtv.ptr += increment;
			}
		}
	};
}
