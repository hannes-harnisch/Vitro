module;
#include "D3D12.API.hh"
export module vt.D3D12.RenderTarget;

import vt.Core.FixedList;
import vt.D3D12.Utils;
import vt.Graphics.RenderPassInfo;
import vt.Graphics.RenderTargetInfo;

namespace vt::d3d12
{
	export class RenderTarget
	{
	public:
		RenderTarget(ComUnique<ID3D12Resource> resource, D3D12_CPU_DESCRIPTOR_HANDLE handle)
		{
			attachments.emplace_back(std::move(resource));
			views.emplace_back(handle);
		}

		ID3D12Resource* get_attachment(size_t index) const
		{
			return attachments[index].get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_view(size_t index) const
		{
			return views[index];
		}

		ID3D12Resource* depth_stencil_attachment() const
		{
			return attachments.back().get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_view() const
		{
			return views.back();
		}

	private:
		FixedList<ComUnique<ID3D12Resource>, MaxAttachments>   attachments;
		FixedList<D3D12_CPU_DESCRIPTOR_HANDLE, MaxAttachments> views;
	};
}
