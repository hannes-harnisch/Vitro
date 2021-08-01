export module Vitro.Graphics.Handle;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.Handle;
#endif
import Vitro.VT_GPU_API_MODULE.Handle;

namespace vt
{
#if VT_DYNAMIC_GPU_API
	#define EXPORT_HANDLE(resource)                                                                                            \
		export using resource##Handle = Handle<VT_GPU_API_NAME_PRIMARY::resource##Ptr, VT_GPU_API_NAME::resource##Ptr>
#else
	#define EXPORT_RESOURCE_HANDLE(resource) export using resource##Handle = Handle<VT_GPU_API_NAME::resource##Ptr>
#endif

	template<
#if VT_DYNAMIC_GPU_API
		typename TImpl1,
#endif
		typename TImpl2>
	struct Handle
	{
		void* handle = nullptr;

		operator bool() const noexcept
		{
			return handle;
		}

#if VT_DYNAMIC_GPU_API
		TImpl1 VT_GPU_API_NAME_PRIMARY() const noexcept
		{
			return static_cast<TImpl1>(handle);
		}
#endif

		TImpl2 VT_GPU_API_NAME() const noexcept
		{
			return static_cast<TImpl2>(handle);
		}
	};

	EXPORT_RESOURCE_HANDLE(Buffer);
	EXPORT_RESOURCE_HANDLE(CommandList);
	EXPORT_RESOURCE_HANDLE(Pipeline);
	EXPORT_RESOURCE_HANDLE(QueryPool);
	EXPORT_RESOURCE_HANDLE(RootSignature);
	EXPORT_RESOURCE_HANDLE(Sampler);
	EXPORT_RESOURCE_HANDLE(Texture);
}
