module;
#include "Core/Macros.hh"
export module vt.Graphics.Sampler;

import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.Sampler;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.Sampler;
#endif

namespace vt
{
	using PlatformSampler = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Sampler)>;
	export class Sampler : public PlatformSampler
	{
	public:
		Sampler(Device const& device, SamplerSpecification const& spec) : PlatformSampler(device, spec)
		{}
	};
}
