module;
#include <string>
#include <typeinfo>
export module Vitro.App.Event;

import Vitro.Core.Reflect;

export class Event
{
public:
	virtual ~Event() = default;

	virtual std::string toString() const
	{
		return nameOf(*this);
	}

protected:
	Event() = default;
};
