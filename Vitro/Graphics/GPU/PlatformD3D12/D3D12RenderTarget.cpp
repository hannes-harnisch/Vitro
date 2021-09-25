module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.RenderTarget;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.AssetResource;
import vt.Graphics.Device;
import vt.Graphics.RenderTargetSpecification;

namespace vt::d3d12
{
	export class D3D12RenderTarget
	{
	public:
		D3D12RenderTarget(Device& in_device, RenderTargetSpecification const& spec) : device(in_device.d3d12)
		{
			unsigned extra		= spec.swap_chain ? 1 : 0;
			unsigned view_count = count(spec.color_attachments) + extra;

			FixedList<ID3D12Resource*, MaxColorAttachments> resources;
			for(auto attachment : spec.color_attachments)
				resources.emplace_back(attachment->d3d12.ptr());

			if(spec.swap_chain)
			{
				auto back_buffer = spec.swap_chain->d3d12.get_back_buffer_ptr(spec.swap_chain_src_index);
				resources.emplace(resources.begin() + spec.swap_chain_dst_index, back_buffer);
			}

			auto	   rtv		 = device.allocate_render_target_views(view_count);
			auto const increment = device.get_render_target_view_size();
			for(auto resource : resources)
			{
				device.ptr()->CreateRenderTargetView(resource, nullptr, rtv.handle);
				textures.emplace_back(resource, rtv.handle);

				rtv.handle.ptr += increment;
			}

			if(spec.depth_stencil_attachment)
			{
				auto ds_resource = spec.depth_stencil_attachment->d3d12.ptr();
				auto dsv		 = device.allocate_depth_stencil_view();

				device.ptr()->CreateDepthStencilView(ds_resource, nullptr, dsv.handle);
				textures.emplace_back(ds_resource, dsv.handle);
			}
			else
				textures.emplace_back(); // Insert null handle so that asking for a depth stencil from outside returns null
		}

		D3D12RenderTarget(D3D12RenderTarget&& that) noexcept : device(that.device), textures(that.textures)
		{
			that.textures.clear();
		}

		~D3D12RenderTarget()
		{
			if(textures.empty())
				return;

			device.deallocate_render_target_views({textures.front().descriptor, count(textures) - 1});
			device.deallocate_depth_stencil_views({textures.back().descriptor, 1});
		}

		void reset(/* TODO */)
		{}

		bool has_depth_stencil() const
		{
			return textures.back().attachment != nullptr;
		}

		ID3D12Resource* get_attachment(size_t index) const
		{
			return textures[index].attachment;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_view(size_t index) const
		{
			return textures[index].descriptor;
		}

		// Returns nullptr if the render target contains no depth stencil attachment.
		ID3D12Resource* get_depth_stencil_attachment() const
		{
			return textures.back().attachment;
		}

		// Returns a null handle if the render target contains no depth stencil attachment.
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const
		{
			return textures.back().descriptor;
		}

		D3D12RenderTarget& operator=(D3D12RenderTarget&&) = delete;

	private:
		D3D12Device& device;

		struct RenderTargetTexture
		{
			ID3D12Resource*				attachment = nullptr;
			D3D12_CPU_DESCRIPTOR_HANDLE descriptor = {};
		};
		FixedList<RenderTargetTexture, MaxAttachments> textures;
	};
}
