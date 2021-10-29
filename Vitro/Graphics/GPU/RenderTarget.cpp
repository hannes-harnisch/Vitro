module;
#include "Core/Macros.hpp"
export module vt.Graphics.RenderTarget;

import vt.Core.Array;
import vt.Graphics.AssetResource;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;
import vt.Graphics.VT_GPU_API_MODULE.RenderTarget;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.RenderTarget;
#endif

namespace vt
{
	using PlatformRenderTarget = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderTarget)>;
	export class RenderTarget : public PlatformRenderTarget
	{
	public:
		// Only intended to be called internally.
		RenderTarget(PlatformRenderTarget platform_render_target, unsigned width, unsigned height) :
			PlatformRenderTarget(std::move(platform_render_target)), width(width), height(height)
		{}

		unsigned get_width() const
		{
			return width;
		}

		unsigned get_height() const
		{
			return height;
		}

	private:
		unsigned width;
		unsigned height;

		void set_width(unsigned new_width)
		{
			width = new_width;
		}

		void set_height(unsigned new_height)
		{
			height = new_height;
		}

		friend class DeviceBase;
	};
}
