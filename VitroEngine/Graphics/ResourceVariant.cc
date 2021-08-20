module;
#include <utility>
export module Vitro.Graphics.ResourceVariant;

namespace vt
{
#if VT_DYNAMIC_GPU_API

	export template<typename TImpl1, typename TImpl2> union ResourceVariant
	{
		TImpl1 VT_GPU_API_NAME_PRIMARY;
		TImpl2 VT_GPU_API_NAME;

		template<typename... Ts> ResourceVariant(Ts&&... ts)
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE_PRIMARY)
				new(&VT_GPU_API_NAME_PRIMARY) TImpl1(std::forward<Ts>(ts)...);
			else
				new(&VT_GPU_API_NAME) TImpl2(std::forward<Ts>(ts)...);
		}

		ResourceVariant(ResourceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE_PRIMARY)
				new(&VT_GPU_API_NAME_PRIMARY) TImpl1(std::move(other.VT_GPU_API_NAME_PRIMARY));
			else
				new(&VT_GPU_API_NAME) TImpl2(std::move(other.VT_GPU_API_NAME));
		}

		~ResourceVariant()
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE_PRIMARY)
				VT_GPU_API_NAME_PRIMARY.~TImpl1();
			else
				VT_GPU_API_NAME.~TImpl2();
		}

		ResourceVariant& operator=(ResourceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE_PRIMARY)
				VT_GPU_API_NAME_PRIMARY = std::move(other.VT_GPU_API_NAME_PRIMARY);
			else
				VT_GPU_API_NAME = std::move(other.VT_GPU_API_NAME);
			return *this;
		}
	};

#else

	export template<typename TImpl> struct ResourceVariant
	{
		TImpl VT_GPU_API_NAME;

		template<typename... Ts> ResourceVariant(Ts&&... ts) : VT_GPU_API_NAME(std::forward<Ts>(ts)...)
		{}
	};

#endif
}
