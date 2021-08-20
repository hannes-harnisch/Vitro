module;
#include "D3D12.API.hh"
export module Vitro.D3D12.RenderTarget;

import Vitro.Core.FixedList;
import Vitro.D3D12.Utils;
import Vitro.Graphics.RenderPassInfo;
import Vitro.Graphics.RenderTargetInfo;

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

		ID3D12Resource* getAttachment(size_t index) const
		{
			return attachments[index].get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE getView(size_t index) const
		{
			return views[index];
		}

		ID3D12Resource* depthStencilAttachment() const
		{
			return attachments.back().get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView() const
		{
			return views.back();
		}

	private:
		FixedList<ComUnique<ID3D12Resource>, MaxAttachments>   attachments;
		FixedList<D3D12_CPU_DESCRIPTOR_HANDLE, MaxAttachments> views;
	};
}
