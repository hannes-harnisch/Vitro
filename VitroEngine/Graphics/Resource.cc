export module vt.Graphics.Resource;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.Buffer;
import vt.VT_GPU_API_MODULE_PRIMARY.Pipeline;
import vt.VT_GPU_API_MODULE_PRIMARY.Texture;
#else
import vt.VT_GPU_API_MODULE.Buffer;
import vt.VT_GPU_API_MODULE.Pipeline;
import vt.VT_GPU_API_MODULE.Texture;
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
	//	EXPORT_RESOURCE_VARIANT(Texture);
}
