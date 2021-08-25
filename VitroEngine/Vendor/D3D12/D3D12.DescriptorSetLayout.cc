module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <span>
#include <vector>
export module vt.D3D12.DescriptorSetLayout;

import vt.Core.Algorithm;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.Device;

namespace vt::d3d12
{
	constexpr D3D12_DESCRIPTOR_RANGE_TYPE convert_descriptor_type(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{
			case Texture: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case StorageBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case UniformBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			case Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}
		VT_UNREACHABLE();
	}

	constexpr D3D12_DESCRIPTOR_RANGE1 convert_descriptor_binding(DescriptorBinding binding)
	{
		return {
			.RangeType						   = convert_descriptor_type(binding.type),
			.NumDescriptors					   = binding.count,
			.BaseShaderRegister				   = binding.slot,
			.RegisterSpace					   = 0,
			.Flags							   = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
			.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		};
	}

	export class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout(vt::Device const&, std::span<DescriptorBinding const> bindings) : ranges(convert_ranges(bindings))
		{}

		D3D12_ROOT_DESCRIPTOR_TABLE1 get_descriptor_table() const
		{
			return {
				.NumDescriptorRanges = count(ranges),
				.pDescriptorRanges	 = ranges.data(),
			};
		}

	private:
		std::vector<D3D12_DESCRIPTOR_RANGE1> ranges;

		static std::vector<D3D12_DESCRIPTOR_RANGE1> convert_ranges(std::span<DescriptorBinding const> bindings)
		{
			std::vector<D3D12_DESCRIPTOR_RANGE1> ranges(bindings.size());

			for(auto binding : bindings)
				ranges.emplace_back(convert_descriptor_binding(binding));

			return ranges;
		}
	};
}
