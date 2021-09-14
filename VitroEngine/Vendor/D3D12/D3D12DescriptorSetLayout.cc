module;
#include "Core/Macros.hh"
#include "D3D12API.hh"

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
		D3D12DescriptorSetLayout(Device const&, DescriptorSetLayoutInfo const& info) :
			is_root_descriptor(is_suitable_as_root_descriptor(info)),
			visibility(convert_shader_stage(visibility)),
			update_frequency(update_frequency)
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

		bool describes_root_descriptor() const
		{
			return is_root_descriptor;
		}

		D3D12_ROOT_DESCRIPTOR_TABLE1 get_descriptor_table_desc() const
		{
			VT_ASSERT(!is_root_descriptor, "This method can only be called on a descriptor set layout corresponding to a "
										   "descriptor table.");
			return {
				.NumDescriptorRanges = count(table_ranges),
				.pDescriptorRanges	 = table_ranges.data(),
			};
		}

		D3D12_ROOT_DESCRIPTOR1 const& get_root_descriptor_desc() const
		{
			return root_descriptor;
		}

		D3D12_SHADER_VISIBILITY get_visibility() const
		{
			return visibility;
		}

		unsigned char get_update_frequency() const
		{
			return update_frequency;
		}

	private:
		union
		{
			std::vector<D3D12_DESCRIPTOR_RANGE1> table_ranges;
			D3D12_ROOT_DESCRIPTOR1				 root_descriptor;
		};
		bool					is_root_descriptor;
		D3D12_SHADER_VISIBILITY visibility : sizeof(char); // TODO: Replace with enum reflection
		unsigned char			update_frequency;

		static bool is_suitable_as_root_descriptor(DescriptorSetLayoutInfo const& info)
		{
			if(info.bindings.size() != 1)
				return false;

			auto& binding = info.bindings.front();
			if(binding.count != 1)
				return false;

			using enum DescriptorType;
			switch(binding.type)
			{
				case Buffer:
				case ByteAddressBuffer:
				case ReadWriteTexture:
				case ReadWriteBuffer:
				case StructuredBuffer:
				case UniformBuffer: return true;
			}
			return false;
		}
	};
}
