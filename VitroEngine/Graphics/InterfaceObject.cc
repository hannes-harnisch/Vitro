module;
#include <new>
#include <utility>
export module Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.Graphics.DynamicGraphicsAPI;

namespace vt
{
	export template<typename TInterface, typename TD3D12, typename TVulkan> union InterfaceObject
	{
		TD3D12 d3d12;
		TVulkan vulkan;

		template<typename... Ts> InterfaceObject(Ts&&... ts)
		{
			if(DynamicGraphicsAPI::isD3D12())
				new(&d3d12) TD3D12(std::forward<Ts>(ts)...);
			else
				new(&vulkan) TVulkan(std::forward<Ts>(ts)...);
		}

		InterfaceObject(InterfaceObject&& other) noexcept
		{
			if(DynamicGraphicsAPI::isD3D12())
				new(&d3d12) TD3D12(std::move(other.d3d12));
			else
				new(&vulkan) TVulkan(std::move(other.vulkan));
		}

		~InterfaceObject()
		{
			if(DynamicGraphicsAPI::isD3D12())
				d3d12.~TD3D12();
			else
				vulkan.~TVulkan();
		}

		InterfaceObject& operator=(InterfaceObject&& other) noexcept
		{
			if(DynamicGraphicsAPI::isD3D12())
				d3d12 = std::move(other.d3d12);
			else
				vulkan = std::move(other.vulkan);
			return *this;
		}

		TInterface* operator->() noexcept
		{
			return std::launder(static_cast<TInterface*>(&d3d12));
		}

		TInterface const* operator->() const noexcept
		{
			return std::launder(static_cast<TInterface const*>(&d3d12));
		}
	};
}

#else

namespace vt
{
	export template<typename TInterface, typename TGraphicsAPI> struct InterfaceObject
	{
		TGraphicsAPI VT_GRAPHICS_API_NAME;

		template<typename... Ts> InterfaceObject(Ts&&... ts) : VT_GRAPHICS_API_NAME(std::forward<Ts>(ts)...)
		{}

		TInterface* operator->() noexcept
		{
			return &VT_GRAPHICS_API_NAME;
		}

		TInterface const* operator->() const noexcept
		{
			return &VT_GRAPHICS_API_NAME;
		}
	};
}

#endif
