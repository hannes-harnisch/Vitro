module;
#include <vector>
export module vt.Graphics.DriverBase;

import vt.Graphics.Handle;

namespace vt
{
	export class DriverBase
	{
	public:
		virtual ~DriverBase() = default;

		virtual std::vector<Adapter> enumerate_adapters() const = 0;
	};
}
