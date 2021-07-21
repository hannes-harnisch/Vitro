export module Vitro.Graphics.DynamicGraphicsAPI;

import Vitro.Core.Singleton;

namespace vt
{
	export enum class GraphicsAPI {
		VT_GRAPHICS_API_MODULE,
#if VT_DYNAMIC_GRAPHICS_API
		VT_GRAPHICS_API_MODULE_PRIMARY
#endif
	};

	export class DynamicGraphicsAPI : public Singleton<DynamicGraphicsAPI>
	{
	public:
		static GraphicsAPI current()
		{
			return get().api;
		}

	private:
		GraphicsAPI api;
	};
}
