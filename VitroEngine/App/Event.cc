module;
#include <string>
#include <typeinfo>
export module vt.App.Event;

import vt.Core.Reflect;

namespace vt
{
	export class Event
	{
	public:
		virtual ~Event() = default;

		virtual std::string to_string() const
		{
			return name_of(*this);
		}

	protected:
		Event() = default;
	};
}
