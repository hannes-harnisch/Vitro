module;
#include <vector>
export module vt.Graphics.DescriptorPoolBase;

import vt.Graphics.DescriptorSet;

namespace vt
{
	export class DescriptorPoolBase
	{
	public:
		virtual ~DescriptorPoolBase() = default;

		virtual std::vector<DescriptorSet> allocate_descriptors() = 0;
	};
}
