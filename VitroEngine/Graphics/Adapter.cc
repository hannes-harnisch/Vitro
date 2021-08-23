module;
#include <string_view>
export module Vitro.Graphics.Adapter;

import Vitro.Graphics.ResourceVariant;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.Adapter;
#endif
import Vitro.VT_GPU_API_MODULE.Adapter;

namespace vt
{
	using AdapterResource = ResourceVariant<
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_NAME_PRIMARY::Adapter,
#endif
		VT_GPU_API_NAME::Adapter>;

	export class Adapter : public AdapterResource
	{
	public:
		Adapter(AdapterResource resource, std::string name, size_t vram) :
			AdapterResource(std::move(resource)), name(std::move(name)), vram(vram)
		{}

		std::string_view getName() const
		{
			return name;
		}

		size_t getVram() const
		{
			return vram;
		}

	private:
		std::string name;
		size_t		vram;
	};
}
