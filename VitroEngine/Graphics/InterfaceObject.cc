module;
#include <new>
#include <utility>
export module Vitro.Graphics.InterfaceObject;

import Vitro.Graphics.DynamicGraphicsAPI;

export template<typename TInterface, typename TD3D12, typename TVulkan> class InterfaceObject
{
public:
	template<typename... Ts> InterfaceObject(Ts&&... ts)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			new(&d3d12) TD3D12(std::forward<Ts>(ts)...);
		else
			new(&vulkan) TVulkan(std::forward<Ts>(ts)...);
	}

	InterfaceObject(InterfaceObject const& other)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			new(&d3d12) TD3D12(other.d3d12);
		else
			new(&vulkan) TVulkan(other.vulkan);
	}

	InterfaceObject(InterfaceObject&& other) noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			new(&d3d12) TD3D12(std::move(other.d3d12));
		else
			new(&vulkan) TVulkan(std::move(other.vulkan));
	}

	~InterfaceObject()
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			d3d12.~TD3D12();
		else
			vulkan.~TVulkan();
	}

	InterfaceObject& operator=(InterfaceObject const& other)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			d3d12 = other.d3d12;
		else
			vulkan = other.vulkan;
		return *this;
	}

	InterfaceObject& operator=(InterfaceObject&& other) noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			d3d12 = std::move(other.d3d12);
		else
			vulkan = std::move(other.vulkan);
		return *this;
	}

	TInterface* operator->() noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			return &d3d12;
		else
			return &vulkan;
	}

	TInterface const* operator->() const noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			return &d3d12;
		else
			return &vulkan;
	}

private:
	union
	{
		TD3D12 d3d12;
		TVulkan vulkan;
	};
};
