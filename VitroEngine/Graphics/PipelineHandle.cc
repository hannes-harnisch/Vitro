export module Vitro.Graphics.PipelineHandle;

import Vitro.D3D12.Pipeline;
// import Vitro.Vulkan.Pipeline;

export union PipelineHandle
{
	D3D12::Pipeline d3d;
	// Vulkan::Pipeline vlk;
};
