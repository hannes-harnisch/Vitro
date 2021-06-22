export module Vitro.Graphics.Handle;

import Vitro.Graphics.DynamicGraphicsAPI;

export template<typename TD3D12, typename TVulkan> union Handle
{
	TD3D12 d3d12;
	TVulkan vulkan;

	void destroy()
	{
		if(DynamicGraphicsAPI::current() == GraphicsAPI::D3D12)
			d3d12.destroy();
		else
			vulkan.destroy();
	}
};
