export module Vitro.Graphics.DescriptorSetLayout;

import Vitro.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.DescriptorSetLayout;
#endif
import Vitro.VT_GPU_API_MODULE.DescriptorSetLayout;

namespace vt
{
	export using DescriptorSetLayout = ResourceVariant<
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_NAME_PRIMARY::DescriptorSetLayout,
#endif
		VT_GPU_API_NAME::DescriptorSetLayout>;
}
