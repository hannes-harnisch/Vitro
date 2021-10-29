module;
#include <vector>
export module vt.Graphics.DriverBase;

import vt.Core.SmallList;
import vt.Graphics.Handle;

namespace vt
{
	export class DriverBase
	{
	public:
		virtual ~DriverBase() = default;

		virtual SmallList<Adapter> enumerate_adapters() const = 0;
	};
}
