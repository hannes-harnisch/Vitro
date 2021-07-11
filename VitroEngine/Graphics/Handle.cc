export module Vitro.Graphics.Handle;

#if VT_DYNAMIC_GRAPHICS_API

	#define EXPORT_RESOURCE_HANDLE(Resource)                                                                                   \
		export using Resource##Handle = Handle<d3d12::Resource##Handle, vulkan::Resource##Handle>;

import Vitro.D3D12.Handle;
import Vitro.Vulkan.Handle;

namespace vt
{
	template<typename TD3D12, typename TVulkan> union Handle
	{
		TD3D12 d3d12;
		TVulkan vulkan;
	};
}

#else

	#define EXPORT_RESOURCE_HANDLE(Resource) export using Resource##Handle = Handle<VT_GRAPHICS_API_NAME::Resource##Handle>;

import Vitro.VT_GRAPHICS_API_MODULE.Handle;

namespace vt
{
	template<typename T> struct Handle
	{
		T VT_GRAPHICS_API_NAME;
	};
}

#endif

namespace vt
{
	EXPORT_RESOURCE_HANDLE(Buffer)
	EXPORT_RESOURCE_HANDLE(Pipeline)
	EXPORT_RESOURCE_HANDLE(RenderPass)
	EXPORT_RESOURCE_HANDLE(RenderTarget)
	EXPORT_RESOURCE_HANDLE(Texture)
}
