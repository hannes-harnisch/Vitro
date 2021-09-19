﻿module;
#include <new>
#include <type_traits>
#include <utility>
export module vt.Graphics.DynamicGpuApi;

import vt.Core.Singleton;

namespace vt
{
	export enum class GpuApi : unsigned char {
		VT_GPU_API_MODULE,
#if VT_DYNAMIC_GPU_API
		VT_GPU_API_MODULE_SECONDARY,
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
		GpuApi api = GpuApi::VT_GPU_API_MODULE;
	};

#if VT_DYNAMIC_GPU_API

	export template<typename I, typename T1, typename T2> union InterfaceVariant
	{
		T1 VT_GPU_API_NAME;
		T2 VT_GPU_API_NAME_SECONDARY;

		template<typename... Ts> InterfaceVariant(Ts&&... ts)
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE)
				std::construct_at(&VT_GPU_API_NAME, std::forward<Ts>(ts)...);
			else
				std::construct_at(&VT_GPU_API_NAME_SECONDARY, std::forward<Ts>(ts)...);
		}

		InterfaceVariant(InterfaceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE)
				std::construct_at(&VT_GPU_API_NAME, std::move(other.VT_GPU_API_NAME));
			else
				std::construct_at(&VT_GPU_API_NAME_SECONDARY, std::move(other.VT_GPU_API_NAME_SECONDARY));
		}

		~InterfaceVariant()
		{
			static_assert(std::has_virtual_destructor_v<I>);
			(*this)->~I();
		}

		InterfaceVariant& operator=(InterfaceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE)
				VT_GPU_API_NAME = std::move(other.VT_GPU_API_NAME);
			else
				VT_GPU_API_NAME_SECONDARY = std::move(other.VT_GPU_API_NAME_SECONDARY);
			return *this;
		}

		I* operator->() noexcept
		{
			return std::launder(static_cast<I*>(&VT_GPU_API_NAME));
		}

		I const* operator->() const noexcept
		{
			return std::launder(static_cast<I const*>(&VT_GPU_API_NAME));
		}
	};

	export template<typename T1, typename T2> union ResourceVariant
	{
		T1 VT_GPU_API_NAME;
		T2 VT_GPU_API_NAME_SECONDARY;

		template<typename... Ts> ResourceVariant(Ts&&... ts)
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE)
				std::construct_at(&VT_GPU_API_NAME, std::forward<Ts>(ts)...);
			else
				std::construct_at(&VT_GPU_API_NAME_SECONDARY, std::forward<Ts>(ts)...);
		}

		ResourceVariant(ResourceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE)
				std::construct_at(&VT_GPU_API_NAME, std::move(other.VT_GPU_API_NAME));
			else
				std::construct_at(&VT_GPU_API_NAME_SECONDARY, std::move(other.VT_GPU_API_NAME_SECONDARY));
		}

		~ResourceVariant()
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE)
				VT_GPU_API_NAME.~T1();
			else
				VT_GPU_API_NAME_SECONDARY.~T2();
		}

		ResourceVariant& operator=(ResourceVariant&& other) noexcept
		{
			if(DynamicGpuApi::current() == GpuApi::VT_GPU_API_MODULE)
				VT_GPU_API_NAME = std::move(other.VT_GPU_API_NAME);
			else
				VT_GPU_API_NAME_SECONDARY = std::move(other.VT_GPU_API_NAME_SECONDARY);
			return *this;
		}
	};

#else

	export template<typename I, typename T> struct InterfaceVariant
	{
		T VT_GPU_API_NAME;

		template<typename... Ts> InterfaceVariant(Ts&&... ts) : VT_GPU_API_NAME(std::forward<Ts>(ts)...)
		{}

		I* operator->() noexcept
		{
			return &VT_GPU_API_NAME;
		}

		I const* operator->() const noexcept
		{
			return &VT_GPU_API_NAME;
		}
	};

	export template<typename T> struct ResourceVariant
	{
		T VT_GPU_API_NAME;

		template<typename... Ts> ResourceVariant(Ts&&... ts) : VT_GPU_API_NAME(std::forward<Ts>(ts)...)
		{}
	};

#endif
}
