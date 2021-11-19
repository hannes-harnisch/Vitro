module;
#include "VitroCore/Macros.hpp"
export module vt.Graphics.Sampler;

import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.Sampler;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Sampler;
#endif

namespace vt
{
	using PlatformSampler = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Sampler)>;
	export class Sampler : public PlatformSampler
	{
		using PlatformSampler::PlatformSampler;
	};
}
