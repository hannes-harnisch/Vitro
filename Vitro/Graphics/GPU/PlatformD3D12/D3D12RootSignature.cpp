module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <format>
#include <stdexcept>
#include <unordered_map>
#include <vector>
export module vt.Graphics.D3D12.RootSignature;

import vt.Core.Array;
import vt.Core.Enum;
import vt.Core.SmallList;
import vt.Graphics.D3D12.DescriptorSetLayout;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Sampler;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;

namespace vt::d3d12
{
	export class RootParameterMap
	{
	public:
		struct IndexPair
		{
			uint8_t view_index;
			uint8_t sampler_table_index;
		};

		void emplace(unsigned layout_id, uint8_t view_index, uint8_t sampler_table_index)
		{
			map.try_emplace(layout_id, view_index, sampler_table_index);
		}

		IndexPair find(unsigned layout_id) const
		{
			return map.find(layout_id)->second;
		}

	private:
		std::unordered_map<unsigned, IndexPair> map;
	};

	export class D3D12RootSignature
	{
	public:
		D3D12RootSignature(RootSignatureSpecification const& spec, ID3D12Device4& device)
		{
			if(spec.push_constants_byte_size % sizeof(DWORD) != 0)
				throw std::invalid_argument(std::format("Push constants byte size must be divisible by {}.", sizeof(DWORD)));

			SmallList<D3D12_ROOT_PARAMETER1> parameters;
			parameters.reserve(D3D12_MAX_ROOT_COST);
			parameters.emplace_back(get_push_constant_range(spec));

			SmallList<D3D12_DESCRIPTOR_RANGE1> ranges;
			ranges.reserve(count_ranges(spec)); // This reserve is crucial to not have dangling pointers.
			SmallList<D3D12_STATIC_SAMPLER_DESC> static_samplers;

			for(uint8_t index = 0; auto& layout : spec.layouts)
			{
				if(layout.d3d12.has_root_descriptor())
				{
					auto& new_parameter = parameters.emplace_back(layout.d3d12.get_root_descriptor_parameter());
					new_parameter.Descriptor.RegisterSpace = index;
				}
				else
				{
					auto view_ranges = layout.d3d12.get_view_descriptor_table_ranges();
					parameters.emplace_back(get_descriptor_table_parameter(ranges, layout, view_ranges, index));
				}
				auto sampler_ranges = layout.d3d12.get_sampler_descriptor_table_ranges();
				if(!sampler_ranges.empty())
					parameters.emplace_back(get_descriptor_table_parameter(ranges, layout, sampler_ranges, index));

				auto descs		= layout.d3d12.get_static_sampler_descs();
				auto desc_begin = static_samplers.insert(static_samplers.end(), descs.begin(), descs.end());
				for(auto it = desc_begin; it != static_samplers.end(); ++it)
					it->RegisterSpace = index;

				// Pre-increment to be offset by 1, because index 0 is reserved for push constants.
				uint8_t view_table_index	= ++index;
				uint8_t sampler_table_index = sampler_ranges.empty() ? 0 : ++index;
				parameter_indices.emplace(layout.d3d12.get_id(), view_table_index, sampler_table_index);
			}

			D3D12_VERSIONED_ROOT_SIGNATURE_DESC const desc {
				.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
				.Desc_1_1 {
					.NumParameters	   = count(parameters),
					.pParameters	   = parameters.data(),
					.NumStaticSamplers = count(static_samplers),
					.pStaticSamplers   = static_samplers.data(),
					.Flags			   = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
				},
			};
			ComUnique<ID3DBlob> signature_blob, error_blob;
			auto result = D3D12SerializeVersionedRootSignature(&desc, std::out_ptr(signature_blob), std::out_ptr(error_blob));
			VT_CHECK_RESULT(result, "Failed to serialize D3D12 root signature.");

			result = device.CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(),
												VT_COM_OUT(root_signature));
			VT_CHECK_RESULT(result, "Failed to create D3D12 root signature.");
		}

		RootParameterMap const& get_parameter_map() const
		{
			return parameter_indices;
		}

		ID3D12RootSignature* get_handle() const
		{
			return root_signature.get();
		}

	private:
		ComUnique<ID3D12RootSignature> root_signature;
		RootParameterMap			   parameter_indices;

		static D3D12_ROOT_PARAMETER1 get_push_constant_range(RootSignatureSpecification const& spec)
		{
			return {
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
				.Constants {
					.ShaderRegister = 0,
					.RegisterSpace	= INT_MAX,
					.Num32BitValues = static_cast<UINT>(spec.push_constants_byte_size / sizeof(DWORD)),
				},
				.ShaderVisibility = SHADER_STAGE_LOOKUP[spec.push_constants_visibility],
			};
		}

		static size_t count_ranges(RootSignatureSpecification const& spec)
		{
			size_t count = 0;

			for(auto& layout : spec.layouts)
				count += layout.d3d12.count_descriptor_table_ranges();

			return count;
		}

		static D3D12_ROOT_PARAMETER1 get_descriptor_table_parameter(SmallList<D3D12_DESCRIPTOR_RANGE1>& tracked,
																	DescriptorSetLayout const&			layout,
																	ConstSpan<D3D12_DESCRIPTOR_RANGE1>	table_ranges,
																	UINT								index)
		{
			auto range_begin = tracked.insert(tracked.end(), table_ranges.begin(), table_ranges.end());
			for(auto it = range_begin; it != tracked.end(); ++it)
				it->RegisterSpace = index;

			return {
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
				.DescriptorTable {
					.NumDescriptorRanges = count(table_ranges),
					.pDescriptorRanges	 = &*range_begin,
				},
				.ShaderVisibility = layout.d3d12.get_visibility(),
			};
		}
	};
}
