module;
#include "VitroCore/Macros.hpp"
export module vt.Graphics.RootSignature;

import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.RootSignature;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.RootSignature;
#endif

namespace vt
{
	using PlatformRootSignature = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RootSignature)>;
	export class RootSignature : public PlatformRootSignature
	{
		using PlatformRootSignature::PlatformRootSignature;
	};
}
