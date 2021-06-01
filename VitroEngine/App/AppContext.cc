export module Vitro.App.AppContext;

import Vitro.VE_SYSTEM.AppContext;

export class AppContext : public VE_SYSTEM::AppContext
{
	friend class AppSystem;

	AppContext() = default;
};
