module;
#include "Core/Macros.hh"

#include <string_view>
export module vt.Graphics.Handle;

import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.Handle;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.Handle;
#endif

namespace vt
{
	using PlatformAdapter = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Adapter)>;
	export class Adapter : public PlatformAdapter
	{
	public:
		Adapter(PlatformAdapter resource, std::string name, size_t vram) :
			PlatformAdapter(std::move(resource)), name(std::move(name)), vram(vram)
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

	export using CommandListHandle = HandleVariant<VT_GPU_API_VARIANT_ARGS(CommandListHandle)>;
	export using SyncValue		   = HandleVariant<VT_GPU_API_VARIANT_ARGS(SyncValue)>;
}
