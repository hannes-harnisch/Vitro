export module Vitro.Trace.Terminal;

import Vitro.VE_SYSTEM.Terminal;

export class Terminal : public VE_SYSTEM::Terminal
{
	friend class Logger;

	Terminal() = default;
};
