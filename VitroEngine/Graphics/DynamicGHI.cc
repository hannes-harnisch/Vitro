export module Vitro.Graphics.DynamicGHI;

import Vitro.Core.Singleton;

namespace vt
{
	export enum class GraphicsHardwareInterface : unsigned char {
		D3D12,
		Vulkan,
	};

	export class DynamicGHI : public Singleton<DynamicGHI>
	{
	public:
		static GraphicsHardwareInterface current()
		{
			return get().api;
		}

		static bool isD3D12()
		{
			return current() == GraphicsHardwareInterface::D3D12;
		}

		static bool isVulkan()
		{
			return current() == GraphicsHardwareInterface::Vulkan;
		}

	private:
		GraphicsHardwareInterface api;
	};
}
