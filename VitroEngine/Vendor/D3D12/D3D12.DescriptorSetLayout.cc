module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <span>
#include <vector>
export module vt.D3D12.DescriptorSetLayout;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.Device;

namespace vt::d3d12
{
	D3D12_DESCRIPTOR_RANGE_TYPE convert_descriptor_type(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{
			case Texture:
			case Buffer:
			case ByteAddressBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case ReadWriteTexture:
			case ReadWriteBuffer:
			case StructuredBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case UniformBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			case DynamicSampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}
		VT_UNREACHABLE();
	}

	export D3D12_SHADER_VISIBILITY convert_shader_stage(ShaderStage stage)
	{
		using enum ShaderStage;
		switch(stage)
		{
			case Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
			case Hull: return D3D12_SHADER_VISIBILITY_HULL;
			case Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
			case Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
			case Fragment: return D3D12_SHADER_VISIBILITY_PIXEL;
			case Task: return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
			case Mesh: return D3D12_SHADER_VISIBILITY_MESH;
			case All:
			case Compute:
			case RayGen:
			case AnyHit:
			case ClosestHit:
			case Miss:
			case Intersection:
			case Callable: return D3D12_SHADER_VISIBILITY_ALL;
		}
		VT_UNREACHABLE();
	}

	export class D3D12DescriptorSetLayout
	{
	public:
		D3D12DescriptorSetLayout(Device const&,
								 CSpan<DescriptorSetBinding> bindings,
								 ShaderStage				 visibility,
								 unsigned					 update_frequency) :
			ranges(bindings.size()), visibility(convert_shader_stage(visibility)), update_frequency(update_frequency)
		{
			for(auto binding : bindings)
				ranges.emplace_back(D3D12_DESCRIPTOR_RANGE1 {
					.RangeType						   = convert_descriptor_type(binding.type),
					.NumDescriptors					   = binding.count,
					.BaseShaderRegister				   = binding.slot,
					.RegisterSpace					   = 0,
					.Flags							   = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
					.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
				});
		}

		D3D12_ROOT_DESCRIPTOR_TABLE1 get_descriptor_table() const
		{
			return {
				.NumDescriptorRanges = count(ranges),
				.pDescriptorRanges	 = ranges.data(),
			};
		}

		D3D12_SHADER_VISIBILITY get_visibility() const
		{
			return visibility;
		}

		unsigned get_update_frequency() const
		{
			return update_frequency;
		}

	private:
		std::vector<D3D12_DESCRIPTOR_RANGE1> ranges;
		D3D12_SHADER_VISIBILITY				 visibility;
		unsigned							 update_frequency;
	};
}
