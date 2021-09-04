module;
#include <span>
export module vt.Graphics.RootSignatureInfo;

import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;

namespace vt
{
	export struct RootSignatureInfo
	{
		unsigned char						   push_constants_size_in_32bit_units;
		ShaderStage							   push_constants_visibility;
		std::span<RootDescriptorBinding const> root_descriptor_bindings;
		std::span<DescriptorSetLayout const>   descriptor_set_layouts;
	};
}
