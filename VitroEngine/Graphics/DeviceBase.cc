export module Vitro.Graphics.DeviceBase;

import Vitro.Graphics.Handle;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual void submitRenderCommands() = 0;
	};
}
