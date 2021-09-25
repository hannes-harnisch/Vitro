module;
#include "Core/Macros.hpp"
export module vt.Graphics.DescriptorPool;

import vt.Core.Array;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorPoolBase;
import vt.Graphics.Device;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.DescriptorPool;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.DescriptorPool;
#endif

namespace vt
{
	using PlatformDescriptorPool = InterfaceVariant<DescriptorPoolBase, VT_GPU_API_VARIANT_ARGS(DescriptorPool)>;
	export class DescriptorPool : public PlatformDescriptorPool
	{
	public:
		DescriptorPool(Device& device, ArrayView<DescriptorCount> descriptor_counts) :
			PlatformDescriptorPool(device, descriptor_counts)
		{}
	};
}
