module;
#include <span>
export module Vitro.Graphics.DeviceBase;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		virtual void submitRenderCommands(std::span<void*> commandLists)  = 0;
		virtual void submitComputeCommands(std::span<void*> commandLists) = 0;
		virtual void submitCopyCommands(std::span<void*> commandLists)	  = 0;
		virtual void flushRenderQueue()									  = 0;
		virtual void flushComputeQueue()								  = 0;
		virtual void flushCopyQueue()									  = 0;
		virtual void waitForIdle()										  = 0;
	};
}
