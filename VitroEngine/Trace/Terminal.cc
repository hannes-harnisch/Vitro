export module Vitro.Trace.Terminal;

import Vitro.VT_SYSTEM.Terminal;

export class Terminal : public VT_SYSTEM::Terminal
{
	friend class Logger;

	Terminal() = default;
};
