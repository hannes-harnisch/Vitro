module;
#include "Core/Macros.hpp"

#include <string>
export module vt.Graphics.Driver;

import vt.Core.Version;
import vt.Graphics.DriverBase;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.Driver;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Driver;
#endif

namespace vt
{
	using PlatformDriver = InterfaceVariant<DriverBase, VT_GPU_API_VARIANT_ARGS(Driver)>;
	export class Driver : public PlatformDriver
	{
	public:
		// This constructor is for internal use only.
		Driver(bool enable_debug_layer, std::string const& app_name, Version app_version, Version engine_version) :
			PlatformDriver(enable_debug_layer, app_name, app_version, engine_version)
		{}
	};
}
