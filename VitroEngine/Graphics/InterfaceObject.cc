module;
#include <new>
#include <utility>
export module Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.Graphics.DynamicGraphicsAPI;

namespace vt
{
	export template<typename TInterface, typename TD3D12Object, typename TVulkanObject> class InterfaceObject
	{
	public:
		template<typename... Ts> InterfaceObject(Ts&&... ts)
		{
			if(DynamicGraphicsAPI::isD3D12())
				new(&d3d12Object) TD3D12Object(std::forward<Ts>(ts)...);
			else
				new(&vulkanObject) TVulkanObject(std::forward<Ts>(ts)...);
		}

		InterfaceObject(InterfaceObject const& other)
		{
			if(DynamicGraphicsAPI::isD3D12())
				new(&d3d12Object) TD3D12Object(other.d3d12Object);
			else
				new(&vulkanObject) TVulkanObject(other.vulkanObject);
		}

		InterfaceObject(InterfaceObject&& other) noexcept
		{
			if(DynamicGraphicsAPI::isD3D12())
				new(&d3d12Object) TD3D12Object(std::move(other.d3d12Object));
			else
				new(&vulkanObject) TVulkanObject(std::move(other.vulkanObject));
		}

		~InterfaceObject()
		{
			if(DynamicGraphicsAPI::isD3D12())
				d3d12Object.~TD3D12();
			else
				vulkanObject.~TVulkan();
		}

		InterfaceObject& operator=(InterfaceObject const& other)
		{
			if(DynamicGraphicsAPI::isD3D12())
				d3d12Object = other.d3d12Object;
			else
				vulkanObject = other.vulkanObject;
			return *this;
		}

		InterfaceObject& operator=(InterfaceObject&& other) noexcept
		{
			if(DynamicGraphicsAPI::isD3D12())
				d3d12Object = std::move(other.d3d12Object);
			else
				vulkanObject = std::move(other.vulkanObject);
			return *this;
		}

		TInterface* operator->() noexcept
		{
			if(DynamicGraphicsAPI::isD3D12())
				return &d3d12Object;
			else
				return &vulkanObject;
		}

		TInterface const* operator->() const noexcept
		{
			if(DynamicGraphicsAPI::isD3D12())
				return &d3d12Object;
			else
				return &vulkanObject;
		}

		TD3D12Object& d3d12() noexcept
		{
			return d3d12Object;
		}

		TD3D12Object const& d3d12() const noexcept
		{
			return d3d12Object;
		}

		TVulkanObject& vulkan() noexcept
		{
			return vulkanObject;
		}

		TVulkanObject const& vulkan() const noexcept
		{
			return vulkanObject;
		}

	private:
		union
		{
			TD3D12Object d3d12Object;
			TVulkanObject vulkanObject;
		};
	};
}

#else

namespace vt
{
	export template<typename TInterface, typename TGHIObject> class InterfaceObject
	{
	public:
		template<typename... Ts> InterfaceObject(Ts&&... ts) : ghi(std::forward<Ts>(ts)...)
		{}

		TInterface* operator->() noexcept
		{
			return &ghi;
		}

		TInterface const* operator->() const noexcept
		{
			return &ghi;
		}

		TGHIObject& VT_GHI_NAME() noexcept
		{
			return ghi;
		}

		TGHIObject const& VT_GHI_NAME() const noexcept
		{
			return ghi;
		}

	private:
		TGHIObject ghi;
	};
}

#endif
