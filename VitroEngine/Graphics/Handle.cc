export module Vitro.Graphics.Handle;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.Handle;
#endif
import Vitro.VT_GPU_API_MODULE.Handle;

namespace vt
{
#if VT_DYNAMIC_GPU_API
	#define EXPORT_HANDLE(object)                                                                                              \
		export using object##Handle = Handle<VT_GPU_API_NAME_PRIMARY::object##Ptr, VT_GPU_API_NAME::object##Ptr>
#else
	#define EXPORT_HANDLE(object) export using object##Handle = Handle<VT_GPU_API_NAME::object##Ptr>
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

	EXPORT_HANDLE(Buffer);
	EXPORT_HANDLE(CommandList);
	EXPORT_HANDLE(Device);
	EXPORT_HANDLE(Pipeline);
	EXPORT_HANDLE(QueryPool);
	EXPORT_HANDLE(Queue);
	EXPORT_HANDLE(RootSignature);
	EXPORT_HANDLE(Sampler);
	EXPORT_HANDLE(Texture);
}
