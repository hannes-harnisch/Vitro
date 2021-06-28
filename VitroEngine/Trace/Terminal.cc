export module Vitro.Trace.Terminal;

import Vitro.VT_SYSTEM_MODULE.Terminal;

namespace vt
{
	export class Terminal : public VT_SYSTEM_NAME::Terminal
	{
		friend class Logger;

		Terminal() = default;
	};
}
