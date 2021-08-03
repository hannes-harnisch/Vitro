export module Vitro.Graphics.Resource;

import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.Device;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineInfo;
import Vitro.Graphics.TextureInfo;

namespace vt
{
	export class RenderPipeline
	{
	public:
		RenderPipeline(Device& device, RenderPipelineInfo const& info) : pipeline(device->makeRenderPipeline(info))
		{}

	private:
		DeferredUnique<PipelineHandle> pipeline;
	};

	export class Texture
	{
	public:
		Texture(Device& device, TextureInfo const& info) : pipeline(device->makeTexture(info))
		{}

	private:
		DeferredUnique<TextureHandle> pipeline;
	};
}
