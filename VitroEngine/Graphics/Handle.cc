export module Vitro.Graphics.Handle;

#if VT_DYNAMIC_GRAPHICS_API
import Vitro.VT_GRAPHICS_API_MODULE_PRIMARY.Handle;
#endif
import Vitro.VT_GRAPHICS_API_MODULE.Handle;

namespace vt
{
#if VT_DYNAMIC_GRAPHICS_API

	#define EXPORT_HANDLE(resource)                                                                                            \
		export using resource##Handle =                                                                                        \
			Handle<VT_GRAPHICS_API_NAME_PRIMARY::Platform##resource##Handle, VT_GRAPHICS_API_NAME::Platform##resource##Handle>

	template<typename TImpl1, typename TImpl2> union Handle
	{
		TImpl1 VT_GRAPHICS_API_NAME_PRIMARY;
		TImpl2 VT_GRAPHICS_API_NAME;
	};

#else

	#define EXPORT_RESOURCE_HANDLE(resource)                                                                                   \
		export using resource##Handle = Handle<VT_GRAPHICS_API_NAME::Platform##resource##Handle>

	template<typename T> struct Handle
	{
		T VT_GRAPHICS_API_NAME;
	};

#endif

	EXPORT_RESOURCE_HANDLE(Buffer);
	EXPORT_RESOURCE_HANDLE(CommandList);
	EXPORT_RESOURCE_HANDLE(Pipeline);
	EXPORT_RESOURCE_HANDLE(RenderPass);
	EXPORT_RESOURCE_HANDLE(RenderTarget);
	EXPORT_RESOURCE_HANDLE(RootSignature);
	EXPORT_RESOURCE_HANDLE(Texture);
}
