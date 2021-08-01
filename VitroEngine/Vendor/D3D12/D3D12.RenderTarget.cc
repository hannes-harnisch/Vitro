module;
#include "D3D12.API.hh"
export module Vitro.D3D12.RenderTarget;

import Vitro.D3D12.Utils;
import Vitro.Graphics.RenderTargetBase;

namespace vt::d3d12
{
	export class RenderTarget : public RenderTargetBase
	{
	public:
		ID3D12Resource* resource() const
		{
			return renderTarget.get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE rtv() const
		{
			return renderTargetView;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE dsv() const
		{
			return depthStencilView;
		}

	private:
		UniqueInterface<ID3D12Resource> renderTarget;
		D3D12_CPU_DESCRIPTOR_HANDLE		renderTargetView;
		D3D12_CPU_DESCRIPTOR_HANDLE		depthStencilView;
	};
}
