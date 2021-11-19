module;
#include "VitroCore/Macros.hpp"
export module vt.Graphics.DescriptorSetLayout;

import vt.Core.Array;
import vt.Core.Specification;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.DescriptorSetLayout;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.DescriptorSetLayout;
#endif

namespace vt
{
	using PlatformDescriptorSetLayout = ResourceVariant<VT_GPU_API_VARIANT_ARGS(DescriptorSetLayout)>;
	export class DescriptorSetLayout : public PlatformDescriptorSetLayout
	{
		using PlatformDescriptorSetLayout::PlatformDescriptorSetLayout;
	};

	export struct RootSignatureSpecification
	{
		Positive<uint8_t>			   push_constants_byte_size;
		Explicit<ShaderStage>		   push_constants_visibility;
		ConstSpan<DescriptorSetLayout> layouts;
	};
}
