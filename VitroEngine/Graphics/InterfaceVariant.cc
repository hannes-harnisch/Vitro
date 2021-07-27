module;
#include <new>
#include <type_traits>
#include <utility>
export module Vitro.Graphics.InterfaceVariant;

import Vitro.Graphics.DynamicGpuApi;

namespace vt
{
#if VT_DYNAMIC_GPU_API

	export template<typename TInterface, typename TImpl1, typename TImpl2> struct InterfaceVariant
	{
		union
		{
			TImpl1 VT_GPU_API_NAME_PRIMARY;
			TImpl2 VT_GPU_API_NAME;
		};

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

#endif
}
