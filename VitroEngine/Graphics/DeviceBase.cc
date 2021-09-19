module;
#include <cstdint>
#include <span>
export module vt.Graphics.DeviceBase;

import vt.Core.Array;
import vt.Graphics.Handle;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		virtual SyncValue submit_render_commands(CSpan<CommandListHandle> command_lists, SyncValue const& gpu_sync = {})  = 0;
		virtual SyncValue submit_compute_commands(CSpan<CommandListHandle> command_lists, SyncValue const& gpu_sync = {}) = 0;
		virtual SyncValue submit_copy_commands(CSpan<CommandListHandle> command_lists, SyncValue const& gpu_sync = {})	  = 0;
		virtual void	  wait_for_workload(SyncValue const& cpu_sync)													  = 0;
		virtual void	  flush_render_queue()																			  = 0;
		virtual void	  flush_compute_queue()																			  = 0;
		virtual void	  flush_copy_queue()																			  = 0;
		virtual void	  wait_for_idle()																				  = 0;
	};
}
