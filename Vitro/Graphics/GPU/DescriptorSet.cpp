module;
#include "Core/Macros.hpp"
export module vt.Graphics.DescriptorSet;

import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.DescriptorSet;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.DescriptorSet;
#endif

namespace vt
{
	using PlatformDescriptorSet = ResourceVariant<VT_GPU_API_VARIANT_ARGS(DescriptorSet)>;

	export class DescriptorSet : public PlatformDescriptorSet
	{};
}
