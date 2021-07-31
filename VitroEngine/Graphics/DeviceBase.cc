module;
#include <span>
export module Vitro.Graphics.DeviceBase;

import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineState;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		virtual DeferredUnique<PipelineHandle> makeRenderPipeline(RenderPipelineState const& state) = 0;
		// virtual DeferredUnique<TextureHandle> makeTexture(TextureState const& state)	 = 0;
		virtual void submitCopyCommands(std::span<CommandListHandle> commands)	  = 0;
		virtual void submitComputeCommands(std::span<CommandListHandle> commands) = 0;
		virtual void submitRenderCommands(std::span<CommandListHandle> commands)  = 0;
	};
}
