module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <atomic>
#include <memory>
#include <vector>
export module vt.Graphics.D3D12.DescriptorSetLayout;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Graphics.DescriptorBinding;

namespace vt::d3d12
{
	D3D12_DESCRIPTOR_RANGE_TYPE convert_descriptor_type(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{ // clang-format off
			case Texture:
			case Buffer:
			case ByteAddressBuffer:	return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case ReadWriteTexture:
			case ReadWriteBuffer:
			case StructuredBuffer:	return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case UniformBuffer:		return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			case DynamicSampler:	return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}
		VT_UNREACHABLE();
	}

	export D3D12_SHADER_VISIBILITY convert_shader_stage(ShaderStage stage)
	{
		using enum ShaderStage;
		switch(stage)
		{
			case Vertex:   return D3D12_SHADER_VISIBILITY_VERTEX;
			case Hull:	   return D3D12_SHADER_VISIBILITY_HULL;
			case Domain:   return D3D12_SHADER_VISIBILITY_DOMAIN;
			case Fragment: return D3D12_SHADER_VISIBILITY_PIXEL;
			case Task:	   return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
			case Mesh:	   return D3D12_SHADER_VISIBILITY_MESH;
			case All:
			case Compute:
			case RayGen:
			case AnyHit:
			case ClosestHit:
			case Miss:
			case Intersection:
			case Callable: return D3D12_SHADER_VISIBILITY_ALL;
		} // clang-format on
		VT_UNREACHABLE();
	}

	export class D3D12DescriptorSetLayout
	{
	public:
		D3D12DescriptorSetLayout(DescriptorSetLayoutSpecification const& spec) :
			parameter_type(determine_parameter_type(spec.bindings)), visibility(convert_shader_stage(spec.visibility))
		{
			if(holds_descriptor_table())
			{
				std::construct_at(&table_ranges);
				table_ranges.reserve(spec.bindings.size());
				for(auto binding : spec.bindings)
					table_ranges.emplace_back(D3D12_DESCRIPTOR_RANGE1 {
						.RangeType						   = convert_descriptor_type(binding.type),
						.NumDescriptors					   = binding.count,
						.BaseShaderRegister				   = binding.shader_register,
						.RegisterSpace					   = binding.space,
						.Flags							   = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
						.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
					});
			}
			else
			{
				auto binding = spec.bindings.front();
				std::construct_at(&root_descriptor, D3D12_ROOT_DESCRIPTOR1 {
														.ShaderRegister = binding.shader_register,
														.RegisterSpace	= binding.space,
														.Flags			= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
													});
			}
		}

		D3D12DescriptorSetLayout(D3D12DescriptorSetLayout&& that) noexcept :
			id(that.id), parameter_type(that.parameter_type), visibility(that.visibility)
		{
			if(holds_descriptor_table())
				std::construct_at(&table_ranges, std::move(that.table_ranges));
			else
				std::construct_at(&root_descriptor, std::move(that.root_descriptor));
		}

		~D3D12DescriptorSetLayout()
		{
			if(holds_descriptor_table())
				table_ranges.~vector();
		}

		D3D12DescriptorSetLayout& operator=(D3D12DescriptorSetLayout&& that) noexcept
		{
			id			   = that.id;
			parameter_type = that.parameter_type;
			visibility	   = that.visibility;

			if(holds_descriptor_table())
				table_ranges = std::move(that.table_ranges);
			else
				root_descriptor = std::move(that.root_descriptor);

			return *this;
		}

		unsigned get_id() const
		{
			return id;
		}

		D3D12_ROOT_PARAMETER1 get_root_parameter() const
		{
			if(holds_descriptor_table())
				return {
					.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					.DescriptorTable {
						.NumDescriptorRanges = count(table_ranges),
						.pDescriptorRanges	 = table_ranges.data(),
					},
					.ShaderVisibility = visibility,
				};
			else
				return {
					.ParameterType	  = parameter_type,
					.Descriptor		  = root_descriptor,
					.ShaderVisibility = visibility,
				};
		}

		UINT get_register_space(size_t table_index) const
		{
			if(holds_descriptor_table())
				return table_ranges[table_index].RegisterSpace;
			else
				return root_descriptor.RegisterSpace;
		}

	private:
		static inline std::atomic_uint id_counter = 0;

		unsigned				  id = id_counter++;
		D3D12_ROOT_PARAMETER_TYPE parameter_type : sizeof(char); // TODO: Replace with enum reflection
		D3D12_SHADER_VISIBILITY	  visibility : sizeof(char);
		union
		{
			std::vector<D3D12_DESCRIPTOR_RANGE1> table_ranges;
			D3D12_ROOT_DESCRIPTOR1				 root_descriptor;
		};

		static D3D12_ROOT_PARAMETER_TYPE determine_parameter_type(ArrayView<DescriptorSetBinding> bindings)
		{
			if(bindings.size() > 1)
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			auto binding = bindings.front();
			if(binding.count > 1)
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			using enum DescriptorType;
			switch(binding.type)
			{
				case Buffer:
				case ByteAddressBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
				case ReadWriteTexture:
				case ReadWriteBuffer:
				case StructuredBuffer: return D3D12_ROOT_PARAMETER_TYPE_UAV;
				case UniformBuffer: return D3D12_ROOT_PARAMETER_TYPE_CBV;
			}
			return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		}

		bool holds_descriptor_table() const
		{
			return parameter_type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		}
	};
}
