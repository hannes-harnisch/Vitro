module;
#include <span>
export module Vitro.Graphics.DeviceBase;

import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineDescription;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		virtual DeferredUnique<PipelineHandle> makeRenderPipeline(RenderPipelineDescription const& desc) = 0;
		// virtual DeferredUnique<TextureHandle> makeTexture(TextureDescription const& desc)	 = 0;
		virtual void submitCopyCommands(std::span<CommandListHandle> commands)	  = 0;
		virtual void submitComputeCommands(std::span<CommandListHandle> commands) = 0;
		virtual void submitRenderCommands(std::span<CommandListHandle> commands)  = 0;
	};
}
