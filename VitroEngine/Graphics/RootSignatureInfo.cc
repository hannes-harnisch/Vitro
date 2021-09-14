module;
#include <span>
export module vt.Graphics.RootSignatureInfo;

import vt.Core.Array;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.SamplerInfo;

namespace vt
{
	export struct RootSignatureInfo
	{
		unsigned char			   push_constants_size_in_32bit_units;
		ShaderStage				   push_constants_visibility;
		CSpan<DescriptorSetLayout> layouts;
		CSpan<StaticSamplerInfo>   static_samplers;
	};
}
