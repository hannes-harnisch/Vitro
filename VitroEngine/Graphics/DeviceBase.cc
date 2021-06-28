export module Vitro.Graphics.DeviceBase;

import Vitro.Graphics.Handle;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual void destroyBuffer(BufferHandle buffer) const		= 0;
		virtual void destroyPipeline(PipelineHandle pipeline) const = 0;
		virtual void destroyTexture(TextureHandle texture) const	= 0;
	};
}
