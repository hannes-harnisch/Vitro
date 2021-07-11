export module Vitro.Graphics.DynamicGraphicsAPI;

import Vitro.Core.Singleton;

namespace vt
{
	export enum class GraphicsAPI { VT_DYNAMIC_GRAPHICS_API_MODULE, VT_DYNAMIC_GRAPHICS_API_MODULE_PRIMARY };

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
