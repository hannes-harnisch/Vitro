module;
#include "D3D12API.hh"
export module vt.D3D12.RenderTarget;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.D3D12.Handle;
import vt.Graphics.AssetResource;
import vt.Graphics.Device;
import vt.Graphics.RenderPass;
import vt.Graphics.TextureSpecification;

namespace vt::d3d12
{
	export class D3D12RenderTarget
	{
	public:
		D3D12RenderTarget(Device const& device,
						  RenderPass const&,
						  ArrayView<Texture const*> color_attachments,
						  Texture const*			depth_stencil_attachment)
		{
			for(auto attachment : color_attachments)
			{}
			if(depth_stencil_attachment)
			{
				textures.emplace_back(depth_stencil_attachment->d3d12.ptr());
			}
			else
				textures.emplace_back(nullptr, 0);
		}

		D3D12RenderTarget(Device const& device, RenderPass const&)
		{
			// Emplace nullptr/null handle at back if depth is null
		}

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
			return textures[index].view;
		}

		// Returns nullptr if the render target contains no depth stencil attachment.
		ID3D12Resource* get_depth_stencil_attachment() const
		{
			return textures.back().attachment;
		}

		// Returns a null handle if the render target contains no depth stencil attachment.
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_stencil_view() const
		{
			return textures.back().view;
		}

	private:
		struct RenderTargetTexture
		{
			ID3D12Resource*				attachment;
			D3D12_CPU_DESCRIPTOR_HANDLE view;
		};
		FixedList<RenderTargetTexture, MaxAttachments> textures;
	};
}
