module;
#include <new>
#include <utility>
export module Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.Graphics.DynamicGraphicsAPI;

export template<typename TInterface, typename TD3D12, typename TVulkan> class InterfaceObject
{
public:
	template<typename... Ts> InterfaceObject(Ts&&... ts)
	{
		if(DynamicGraphicsAPI::isD3D12())
			new(&d3d12Object) TD3D12(std::forward<Ts>(ts)...);
		else
			new(&vulkanObject) TVulkan(std::forward<Ts>(ts)...);
	}

	InterfaceObject(InterfaceObject const& other)
	{
		if(DynamicGraphicsAPI::isD3D12())
			new(&d3d12Object) TD3D12(other.d3d12Object);
		else
			new(&vulkanObject) TVulkan(other.vulkanObject);
	}

	InterfaceObject(InterfaceObject&& other) noexcept
	{
		if(DynamicGraphicsAPI::isD3D12())
			new(&d3d12Object) TD3D12(std::move(other.d3d12Object));
		else
			new(&vulkanObject) TVulkan(std::move(other.vulkanObject));
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

	TD3D12& d3d12() noexcept
	{
		return d3d12Object;
	}

	TD3D12 const& d3d12() const noexcept
	{
		return d3d12Object;
	}

	TVulkan& vulkan() noexcept
	{
		return vulkanObject;
	}

	TVulkan const& vulkan() const noexcept
	{
		return vulkanObject;
	}

private:
	union
	{
		TD3D12 d3d12Object;
		TVulkan vulkanObject;
	};
};

#else

export template<typename TInterface, typename TPlatform> class InterfaceObject
{
public:
	template<typename... Ts> InterfaceObject(Ts&&... ts) : object(std::forward<Ts>(ts)...)
	{}

	TInterface* operator->() noexcept
	{
		return &object;
	}

	TInterface const* operator->() const noexcept
	{
		return &object;
	}

	TPlatform& VT_GHI_LOWER_CASE() noexcept
	{
		return object;
	}

	TPlatform const& VT_GHI_LOWER_CASE() const noexcept
	{
		return object;
	}

private:
	TPlatform object;
};

#endif
