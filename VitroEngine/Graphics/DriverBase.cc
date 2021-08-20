﻿module;
#include <vector>
export module Vitro.Graphics.DriverBase;

import Vitro.Graphics.Adapter;

namespace vt
{
	export class DriverBase
	{
	public:
		virtual ~DriverBase() = default;

		virtual std::vector<Adapter> enumerateAdapters() const = 0;
	};
}
