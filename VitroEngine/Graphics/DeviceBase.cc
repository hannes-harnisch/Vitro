module;
#include <cstdint>
#include <span>
export module vt.Graphics.DeviceBase;

import vt.Core.Array;
import vt.Graphics.Driver;

namespace vt
{
	export struct Receipt
	{
		std::uintptr_t host_wait_value = 0;
	};

	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		virtual Receipt submit_render_commands(CSpan<CommandListHandle> command_lists)	= 0;
		virtual Receipt submit_compute_commands(CSpan<CommandListHandle> command_lists) = 0;
		virtual Receipt submit_copy_commands(CSpan<CommandListHandle> command_lists)	= 0;
		virtual void	wait_for_render_workload(Receipt receipt)						= 0;
		virtual void	wait_for_compute_workload(Receipt receipt)						= 0;
		virtual void	wait_for_copy_workload(Receipt receipt)							= 0;
		virtual void	flush_render_queue()											= 0;
		virtual void	flush_compute_queue()											= 0;
		virtual void	flush_copy_queue()												= 0;
		virtual void	wait_for_idle()													= 0;
	};
}
