﻿module;
#include <vector>
export module vt.Graphics.DescriptorPoolBase;

import vt.Core.Array;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.RootSignature;

namespace vt
{
	export class DescriptorPoolBase
	{
	public:
		virtual ~DescriptorPoolBase() = default;

		virtual std::vector<DescriptorSet> allocate_descriptors(CSpan<DescriptorSetLayout> layouts,
																RootSignature const&	   root_signature) = 0;

		virtual void reset() = 0;
	};
}
