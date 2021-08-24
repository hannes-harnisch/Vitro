module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <span>
#include <vector>
export module Vitro.D3D12.DescriptorSetLayout;

import Vitro.Core.Algorithm;
import Vitro.Graphics.DescriptorBinding;
import Vitro.Graphics.Device;

namespace vt::d3d12
{
	constexpr D3D12_DESCRIPTOR_RANGE_TYPE convertDescriptorType(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{
			case Texture: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case StorageBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case UniformBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			case Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}
		vtUnreachable();
	}

	constexpr D3D12_DESCRIPTOR_RANGE1 convertDescriptorBinding(DescriptorBinding binding)
	{
		return D3D12_DESCRIPTOR_RANGE1 {
			.RangeType						   = convertDescriptorType(binding.type),
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
		DescriptorSetLayout(vt::Device const&, std::span<DescriptorBinding const> bindings) : ranges(convertRanges(bindings))
		{}

		D3D12_ROOT_DESCRIPTOR_TABLE1 getDescriptorTable() const
		{
			return {
				.NumDescriptorRanges = count(ranges),
				.pDescriptorRanges	 = ranges.data(),
			};
		}

	private:
		std::vector<D3D12_DESCRIPTOR_RANGE1> ranges;

		static std::vector<D3D12_DESCRIPTOR_RANGE1> convertRanges(std::span<DescriptorBinding const> bindings)
		{
			std::vector<D3D12_DESCRIPTOR_RANGE1> ranges(bindings.size());

			for(auto binding : bindings)
				ranges.emplace_back(convertDescriptorBinding(binding));

			return ranges;
		}
	};
}
