module;
#include "Core/Macros.hpp"
export module vt.Graphics.Shader;

import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.Shader;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Shader;
#endif

namespace vt
{
	using PlatformShader = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Shader)>;
	export class Shader : public PlatformShader
	{
		using PlatformShader::PlatformShader;
	};
}
