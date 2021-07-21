module;
#include <span>
export module Vitro.Graphics.DeviceBase;

import Vitro.Graphics.Handle;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual void submitCopyCommands(std::span<CommandListHandle const> commands)	= 0;
		virtual void submitComputeCommands(std::span<CommandListHandle const> commands) = 0;
		virtual void submitRenderCommands(std::span<CommandListHandle const> commands)	= 0;
	};
}
