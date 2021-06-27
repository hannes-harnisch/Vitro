export module Vitro.Graphics.DeviceBase;

import Vitro.Graphics.Handle;

export class DeviceBase
{
public:
	virtual void destroyBuffer(BufferHandle buffer)		  = 0;
	virtual void destroyPipeline(PipelineHandle pipeline) = 0;
	virtual void destroyTexture(TextureHandle texture)	  = 0;
};
