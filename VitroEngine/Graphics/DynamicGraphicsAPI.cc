export module Vitro.Graphics.DynamicGraphicsAPI;

import Vitro.Core.Singleton;

namespace vt
{
	export enum class GraphicsAPI : unsigned char {
		D3D12,
		Vulkan,
	};

	export class DynamicGraphicsAPI : public Singleton<DynamicGraphicsAPI>
	{
	public:
		static GraphicsAPI current()
		{
			return get().api;
		}

		static bool isD3D12()
		{
			return current() == GraphicsAPI::D3D12;
		}

		static bool isVulkan()
		{
			return current() == GraphicsAPI::Vulkan;
		}

	private:
		GraphicsAPI api;
	};
}
