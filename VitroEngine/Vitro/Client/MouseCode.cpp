#include "_pch.h"
#include "MouseCode.h"

#define VTR_STRING_CONVERSION_CASE(X)                                                                                  \
	case MouseCode::X: return #X

namespace Vitro
{
	std::string ToString(MouseCode button)
	{
		switch(button)
		{
			VTR_STRING_CONVERSION_CASE(None);
			VTR_STRING_CONVERSION_CASE(Mouse1);
			VTR_STRING_CONVERSION_CASE(Mouse2);
			VTR_STRING_CONVERSION_CASE(Wheel);
			VTR_STRING_CONVERSION_CASE(Extra1);
			VTR_STRING_CONVERSION_CASE(Extra2);
		}
		return "Unknown mouse button: " + std::to_string(static_cast<int>(button));
	}
}
