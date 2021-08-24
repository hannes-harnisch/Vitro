module;
#include <new>
#include <type_traits>
#include <utility>
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

#if VT_DYNAMIC_GPU_API

	export template<typename TInterface, typename TImpl1, typename TImpl2> union InterfaceVariant
	{
		TImpl1 VT_GPU_API_NAME_PRIMARY;
		TImpl2 VT_GPU_API_NAME;

		template<typename... Ts> InterfaceVariant(Ts&&... ts)
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE_PRIMARY)
				new(&VT_GPU_API_NAME_PRIMARY) TImpl1(std::forward<Ts>(ts)...);
			else
				new(&VT_GPU_API_NAME) TImpl2(std::forward<Ts>(ts)...);
		}

		InterfaceVariant(InterfaceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE_PRIMARY)
				new(&VT_GPU_API_NAME_PRIMARY) TImpl1(std::move(other.VT_GPU_API_NAME_PRIMARY));
			else
				new(&VT_GPU_API_NAME) TImpl2(std::move(other.VT_GPU_API_NAME));
		}

		~InterfaceVariant()
		{
			static_assert(std::has_virtual_destructor_v<TInterface>);
			(*this)->~TInterface();
		}

		InterfaceVariant& operator=(InterfaceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE_PRIMARY)
				VT_GPU_API_NAME_PRIMARY = std::move(other.VT_GPU_API_NAME_PRIMARY);
			else
				VT_GPU_API_NAME = std::move(other.VT_GPU_API_NAME);
			return *this;
		}

		TInterface* operator->() noexcept
		{
			return std::launder(static_cast<TInterface*>(&VT_GPU_API_NAME_PRIMARY));
		}

		TInterface const* operator->() const noexcept
		{
			return std::launder(static_cast<TInterface const*>(&VT_GPU_API_NAME_PRIMARY));
		}
	};

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

	export template<typename TInterface, typename TImpl> struct InterfaceVariant
	{
		TImpl VT_GPU_API_NAME;

		template<typename... Ts> InterfaceVariant(Ts&&... ts) : VT_GPU_API_NAME(std::forward<Ts>(ts)...)
		{}

		TInterface* operator->() noexcept
		{
			return &VT_GPU_API_NAME;
		}

		TInterface const* operator->() const noexcept
		{
			return &VT_GPU_API_NAME;
		}
	};

	export template<typename TImpl> struct ResourceVariant
	{
		TImpl VT_GPU_API_NAME;

		template<typename... Ts> ResourceVariant(Ts&&... ts) : VT_GPU_API_NAME(std::forward<Ts>(ts)...)
		{}
	};

#endif
}
