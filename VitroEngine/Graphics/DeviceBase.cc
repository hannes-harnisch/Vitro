module;
#include <cstdint>
#include <span>
export module Vitro.Graphics.DeviceBase;

namespace vt
{
	export struct Receipt
	{
		std::uintptr_t hostWaitValue = 0;
	};

	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		virtual Receipt submitRenderCommands(std::span<void*> commandLists)	 = 0;
		virtual Receipt submitComputeCommands(std::span<void*> commandLists) = 0;
		virtual Receipt submitCopyCommands(std::span<void*> commandLists)	 = 0;
		virtual void	waitForRenderWorkload(Receipt receipt)				 = 0;
		virtual void	waitForComputeWorkload(Receipt receipt)				 = 0;
		virtual void	waitForCopyWorkload(Receipt receipt)				 = 0;
		virtual void	flushRenderQueue()									 = 0;
		virtual void	flushComputeQueue()									 = 0;
		virtual void	flushCopyQueue()									 = 0;
		virtual void	waitForIdle()										 = 0;
	};
}
