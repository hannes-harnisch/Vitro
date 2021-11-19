module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Sampler;

import vt.Core.LookupTable;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.D3D12.DescriptorAllocator;

namespace vt::d3d12
{
	export constexpr inline auto COMPARE_OP_LOOKUP = [] {
		LookupTable<CompareOp, D3D12_COMPARISON_FUNC> _;
		using enum CompareOp;

		_[Never]		  = D3D12_COMPARISON_FUNC_NEVER;
		_[Less]			  = D3D12_COMPARISON_FUNC_LESS;
		_[Equal]		  = D3D12_COMPARISON_FUNC_EQUAL;
		_[LessOrEqual]	  = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		_[Greater]		  = D3D12_COMPARISON_FUNC_GREATER;
		_[NotEqual]		  = D3D12_COMPARISON_FUNC_NOT_EQUAL;
		_[GreaterOrEqual] = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		_[Always]		  = D3D12_COMPARISON_FUNC_ALWAYS;
		return _;
	}();

	export constexpr inline auto SHADER_STAGE_LOOKUP = [] {
		LookupTable<ShaderStage, D3D12_SHADER_VISIBILITY> _;
		using enum ShaderStage;

		_[Vertex]		= D3D12_SHADER_VISIBILITY_VERTEX;
		_[Hull]			= D3D12_SHADER_VISIBILITY_HULL;
		_[Domain]		= D3D12_SHADER_VISIBILITY_DOMAIN;
		_[Fragment]		= D3D12_SHADER_VISIBILITY_PIXEL;
		_[Task]			= D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		_[Mesh]			= D3D12_SHADER_VISIBILITY_MESH;
		_[All]			= D3D12_SHADER_VISIBILITY_ALL;
		_[Compute]		= _[All];
		_[RayGen]		= _[All];
		_[AnyHit]		= _[All];
		_[ClosestHit]	= _[All];
		_[Miss]			= _[All];
		_[Intersection] = _[All];
		_[Callable]		= _[All];
		return _;
	}();

	export D3D12_STATIC_SAMPLER_DESC convert_static_sampler_spec(SamplerSpecification const& spec,
																 unsigned					 shader_register,
																 ShaderStage				 visibility);

	export class D3D12Sampler
	{
	public:
		D3D12Sampler(SamplerSpecification const& spec, ID3D12Device4& device, UniqueCpuDescriptor descriptor);

		D3D12_CPU_DESCRIPTOR_HANDLE get_handle() const;

	private:
		UniqueCpuDescriptor sampler;
	};
}
