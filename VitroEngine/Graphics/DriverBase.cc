module;
#include <vector>
export module Vitro.Graphics.DriverBase;

import Vitro.Graphics.Adapter;

export class DriverBase
{
public:
	virtual std::vector<Adapter> enumerateAdapters() const = 0;
};
