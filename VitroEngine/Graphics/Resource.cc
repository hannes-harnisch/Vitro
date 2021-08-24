export module Vitro.Graphics.Resource;

import Vitro.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.Buffer;
import Vitro.VT_GPU_API_MODULE_PRIMARY.Pipeline;
#else
import Vitro.VT_GPU_API_MODULE.Buffer;
import Vitro.VT_GPU_API_MODULE.Pipeline;
#endif

namespace vt
{
#if VT_DYNAMIC_GPU_API
	#define EXPORT_RESOURCE_VARIANT(resource)                                                                                  \
		export using resource = ResourceVariant<VT_GPU_API_NAME_PRIMARY::resource, VT_GPU_API_NAME::resource>
#else
	#define EXPORT_RESOURCE_VARIANT(resource) export using resource = ResourceVariant<VT_GPU_API_NAME::resource>
#endif

	EXPORT_RESOURCE_VARIANT(Buffer);
	EXPORT_RESOURCE_VARIANT(Pipeline);
}
