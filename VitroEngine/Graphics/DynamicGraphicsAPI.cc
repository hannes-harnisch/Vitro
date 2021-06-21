module;
#include <new>
#include <utility>
export module Vitro.Graphics.DynamicGraphicsAPI;

import Vitro.Core.Singleton;

export enum class GraphicsAPI : bool { D3D1212, Vulkan };

export class DynamicGraphicsAPI : public Singleton<DynamicGraphicsAPI>
{
	friend class GraphicsSystem;

public:
	static GraphicsAPI current()
	{
		return get().api;
	}

private:
	GraphicsAPI api;

	DynamicGraphicsAPI() = default;
};

export template<typename TInterface, typename TD3D12, typename TVulkan> class InterfaceVariant
{
public:
	template<typename... Ts> InterfaceVariant(Ts&&... ts)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			new(&d3d) TD3D12(std::forward<Ts>(ts)...);
		else
			new(&vlk) TVulkan(std::forward<Ts>(ts)...);
	}

	InterfaceVariant(InterfaceVariant const& other)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			new(&d3d) TD3D12(other.d3d);
		else
			new(&vlk) TVulkan(other.vlk);
	}

	InterfaceVariant(InterfaceVariant&& other) noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			new(&d3d) TD3D12(std::move(other.d3d));
		else
			new(&vlk) TVulkan(std::move(other.vlk));
	}

	~InterfaceVariant()
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			d3d.~TD3D12();
		else
			vlk.~TVulkan();
	}

	InterfaceVariant& operator=(InterfaceVariant const& other)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			d3d = other.d3d;
		else
			vlk = other.vlk;
		return *this;
	}

	InterfaceVariant& operator=(InterfaceVariant&& other) noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			d3d = std::move(other.d3d);
		else
			vlk = std::move(other.vlk);
		return *this;
	}

	TInterface* operator->() noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			return &d3d;
		else
			return &vlk;
	}

	TInterface const* operator->() const noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D1212)
			return &d3d;
		else
			return &vlk;
	}

private:
	union
	{
		TD3D12 d3d;
		TVulkan vlk;
	};
};
