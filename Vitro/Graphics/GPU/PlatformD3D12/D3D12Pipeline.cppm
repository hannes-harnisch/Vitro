module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Pipeline;

import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Sampler;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.Shader;

namespace vt::d3d12
{
	class Pipeline
	{
	public:
		ID3D12PipelineState* get_handle() const;

	protected:
		ComUnique<ID3D12PipelineState> pipeline;
	};

	export class D3D12RenderPipeline : public Pipeline
	{
	public:
		D3D12RenderPipeline(RenderPipelineSpecification const& spec, ID3D12Device4& device);

		D3D_PRIMITIVE_TOPOLOGY get_topology() const;

	private:
		D3D_PRIMITIVE_TOPOLOGY primitive_topology;
	};

	export class D3D12ComputePipeline : public Pipeline
	{
	public:
		D3D12ComputePipeline(ComputePipelineSpecification const& spec, ID3D12Device4& device);
	};
}
