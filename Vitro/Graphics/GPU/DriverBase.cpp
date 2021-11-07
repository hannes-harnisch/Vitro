module;
#include <vector>
export module vt.Graphics.DriverBase;

import vt.Core.SmallList;
import vt.Graphics.Device;
import vt.Graphics.Handle;

namespace vt
{
	export class DriverBase
	{
	public:
		virtual ~DriverBase() = default;

		// Returns a list of all GPUs available on the system.
		virtual SmallList<Adapter> enumerate_adapters() const = 0;

		// Make a device object connected to the given adapter.
		virtual Device make_device(Adapter const& adapter) const = 0;
	};
}
