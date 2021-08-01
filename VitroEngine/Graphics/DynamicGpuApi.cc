export module Vitro.Graphics.DynamicGpuApi;

import Vitro.Core.Singleton;

namespace vt
{
	export enum class GpuApi {
		VT_GPU_API_MODULE,
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_MODULE_PRIMARY
#endif
	};

	export class DynamicGpuApi : public Singleton<DynamicGpuApi>
	{
	public:
		static GpuApi current()
		{
			return get().api;
		}

	private:
		GpuApi api;
	};
}
