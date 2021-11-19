module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <algorithm>
#include <atomic>
#include <unordered_map>
export module vt.Graphics.D3D12.DescriptorSetLayout;

import vt.Core.Array;
import vt.Core.LookupTable;
import vt.Core.SmallList;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.D3D12.Sampler;

namespace vt::d3d12
{
	struct ShaderRegister
	{
		D3D12_DESCRIPTOR_RANGE_TYPE type;
		unsigned					register_number;

		bool operator==(ShaderRegister const&) const = default;
	};
}

template<> struct std::hash<vt::d3d12::ShaderRegister>
{
	size_t operator()(vt::d3d12::ShaderRegister shader_register) const
	{
		return std::bit_cast<size_t>(shader_register);
	}
};

namespace vt::d3d12
{
	export using RangeOffsetMap = std::unordered_map<ShaderRegister, unsigned>;

	export class D3D12DescriptorSetLayout
	{
	public:
		D3D12DescriptorSetLayout(DescriptorSetLayoutSpecification const& spec);

		unsigned							 get_id() const;
		D3D12_SHADER_VISIBILITY				 get_visibility() const;
		D3D12_ROOT_PARAMETER_TYPE			 get_view_root_parameter_type() const;
		ConstSpan<D3D12_STATIC_SAMPLER_DESC> get_static_sampler_descs() const;
		bool								 has_root_descriptor() const;
		ConstSpan<D3D12_DESCRIPTOR_RANGE1>	 get_view_descriptor_table_ranges() const;
		ConstSpan<D3D12_DESCRIPTOR_RANGE1>	 get_sampler_descriptor_table_ranges() const;
		size_t								 count_descriptor_table_ranges() const;
		D3D12_ROOT_PARAMETER1				 get_root_descriptor_parameter() const;
		unsigned							 get_range_offset(D3D12_DESCRIPTOR_RANGE_TYPE type, unsigned register_number) const;

	private:
		static std::atomic_uint id_counter;

		unsigned						 id = id_counter++;
		D3D12_SHADER_VISIBILITY			 visibility;
		UINT							 root_descriptor_register;
		D3D12_ROOT_PARAMETER_TYPE		 view_parameter_type : sizeof(char);
		uint8_t							 sampler_range_start_index;
		Array<D3D12_STATIC_SAMPLER_DESC> static_samplers;
		Array<D3D12_DESCRIPTOR_RANGE1>	 descriptor_table_ranges;
		RangeOffsetMap					 range_offsets;

		size_t determine_descriptor_table_range_count(DescriptorSetLayoutSpecification const& spec) const;
		void   initialize_static_samplers(DescriptorSetLayoutSpecification const& spec);
		void   initialize_descriptor_table_ranges(DescriptorSetLayoutSpecification const& spec);
	};
}
