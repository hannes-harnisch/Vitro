export module Vitro.Graphics.DynamicGraphicsAPI;

import Vitro.Core.Singleton;

export enum class GraphicsAPI : bool { D3D12, Vulkan };

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
