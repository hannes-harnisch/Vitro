module;
#include <span>
export module Vitro.Graphics.DeviceBase;

import Vitro.Graphics.Handle;
import Vitro.Graphics.Pipeline;
import Vitro.Graphics.Texture;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual void submitCopyCommands(std::span<CommandListHandle const> commands)	= 0;
		virtual void submitComputeCommands(std::span<CommandListHandle const> commands) = 0;
		virtual void submitRenderCommands(std::span<CommandListHandle const> commands)	= 0;
		//	virtual Pipeline makePipeline(PipelineInfo const& info)							= 0;
		//	virtual Texture makeTexture(TextureInfo const& info)							= 0;
	};
}
