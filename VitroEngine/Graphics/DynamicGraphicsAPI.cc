module;
#include <new>
#include <utility>
export module Vitro.Graphics.DynamicGraphicsAPI;

import Vitro.Core.Singleton;

export enum class GraphicsAPI : bool { Direct3D, Vulkan };

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

export template<typename TInterface, typename TDirect3D, typename TVulkan> class InterfaceVariant
{
public:
	template<typename... Ts> InterfaceVariant(Ts&&... ts)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			new(&d3d) TDirect3D(std::forward<Ts>(ts)...);
		else
			new(&vlk) TVulkan(std::forward<Ts>(ts)...);
	}

	InterfaceVariant(InterfaceVariant const& other)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			new(&d3d) TDirect3D(other.d3d);
		else
			new(&vlk) TVulkan(other.vlk);
	}

	InterfaceVariant(InterfaceVariant&& other) noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			new(&d3d) TDirect3D(std::move(other.d3d));
		else
			new(&vlk) TVulkan(std::move(other.vlk));
	}

	~InterfaceVariant()
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			d3d.~TDirect3D();
		else
			vlk.~TVulkan();
	}

	InterfaceVariant& operator=(InterfaceVariant const& other)
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			d3d = other.d3d;
		else
			vlk = other.vlk;
		return *this;
	}

	InterfaceVariant& operator=(InterfaceVariant&& other) noexcept
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			d3d = std::move(other.d3d);
		else
			vlk = std::move(other.vlk);
		return *this;
	}

	TInterface* operator->()
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			return &d3d;
		else
			return &vlk;
	}

	TInterface const* operator->() const
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::Direct3D)
			return &d3d;
		else
			return &vlk;
	}

private:
	union
	{
		TDirect3D d3d;
		TVulkan vlk;
	};
};
