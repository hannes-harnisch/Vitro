module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Device;

import Vitro.D3D12.Unique;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Handle;

namespace D3D12
{
	export class Device final : public DeviceBase
	{
	public:
		void destroyBuffer(BufferHandle const buffer) override
		{
			buffer.d3d12.handle->Release();
		}

		void destroyPipeline(PipelineHandle const pipeline) override
		{
			pipeline.d3d12.handle->Release();
		}

		void destroyTexture(TextureHandle const texture) override
		{
			texture.d3d12.handle->Release();
		}

	private:
		Unique<ID3D12Device> device;
	};
}
