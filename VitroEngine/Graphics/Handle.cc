export module Vitro.Graphics.Handle;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Resource;
import Vitro.Vulkan.Resource;

namespace vt
{
	template<typename TD3D12, typename TVulkan> union Handle
	{
		TD3D12 d3d12;
		TVulkan vulkan;
	};

	export using BufferHandle	= Handle<D3D12::Buffer, Vulkan::Buffer>;
	export using PipelineHandle = Handle<D3D12::Pipeline, Vulkan::Pipeline>;
	export using TextureHandle	= Handle<D3D12::Texture, Vulkan::Texture>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.Resource;

namespace vt
{
	template<typename T> struct Handle
	{
		T VT_GRAPHICS_API_NAME;
	};

	export using BufferHandle	= Handle<VT_GRAPHICS_API_NAME::Buffer>;
	export using PipelineHandle = Handle<VT_GRAPHICS_API_NAME::Pipeline>;
	export using TextureHandle	= Handle<VT_GRAPHICS_API_NAME::Texture>;
}

#endif
