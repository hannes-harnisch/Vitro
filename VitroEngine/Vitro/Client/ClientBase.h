#pragma once

#if VTR_SYSTEM_WINDOWS
	#include "Vitro/API/Windows/ClientBase.h"
#endif

namespace Vitro
{
#if VTR_SYSTEM_WINDOWS
	typedef Windows::ClientBase ClientBase;
#else
	#error Unsupported system.
#endif
}
