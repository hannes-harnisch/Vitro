module;
#include "Core/Macros.hh"

#include <string_view>
export module vt.Graphics.Adapter;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.Adapter;
#endif
import vt.VT_GPU_API_MODULE.Adapter;

namespace vt
{
	export using AdapterResource = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Adapter)>;

	export class Adapter : public AdapterResource
	{
	public:
		Adapter(AdapterResource resource, std::string name, size_t vram) :
			AdapterResource(std::move(resource)), name(std::move(name)), vram(vram)
		{}

		std::string_view get_name() const
		{
			return name;
		}

		size_t get_vram() const
		{
			return vram;
		}

	private:
		std::string name;
		size_t		vram;
	};
}
