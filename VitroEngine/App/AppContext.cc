export module Vitro.App.AppContext;

import Vitro.VT_SYSTEM.AppContext;

export class AppContext : public VT_SYSTEM::AppContext
{
	friend class AppSystem;

	AppContext() = default;
};
