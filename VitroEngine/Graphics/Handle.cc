export module Vitro.Graphics.Handle;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Handle;
import Vitro.Vulkan.Handle;

namespace vt
{
	template<typename TD3D12, typename TVulkan> union Handle
	{
		TD3D12 d3d12;
		TVulkan vulkan;
	};

	export using BufferHandle	= Handle<D3D12::BufferHandle, Vulkan::BufferHandle>;
	export using PipelineHandle = Handle<D3D12::PipelineHandle, Vulkan::PipelineHandle>;
	export using TextureHandle	= Handle<D3D12::TextureHandle, Vulkan::TextureHandle>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.Handle;

namespace vt
{
	template<typename T> struct Handle
	{
		T VT_GRAPHICS_API_NAME;
	};

	export using BufferHandle	= Handle<VT_GRAPHICS_API_NAME::BufferHandle>;
	export using PipelineHandle = Handle<VT_GRAPHICS_API_NAME::PipelineHandle>;
	export using TextureHandle	= Handle<VT_GRAPHICS_API_NAME::TextureHandle>;
}

#endif
