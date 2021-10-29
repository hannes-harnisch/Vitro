module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

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
	export class RootSignatureParameterMap
	{
	public:
		void emplace(unsigned layout_id, uint8_t index)
		{
			map.try_emplace(layout_id, index);
		}

		uint8_t find(unsigned layout_id) const
		{
			return map.find(layout_id)->second;
		}

	private:
		std::unordered_map<unsigned, uint8_t> map;
	};

	export class D3D12RootSignature
	{
	public:
		D3D12RootSignature(ID3D12Device4* device, RootSignatureSpecification const& spec)
		{
			if(spec.push_constants_byte_size % sizeof(DWORD) != 0)
				throw std::invalid_argument(std::format("Push constants byte size must be divisible by {}.", sizeof(DWORD)));

			SmallList<D3D12_ROOT_PARAMETER1> parameters;
			parameters.reserve(D3D12_MAX_ROOT_COST);
			parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
				.Constants {
					.ShaderRegister = 0,
					.RegisterSpace	= 0,
					.Num32BitValues = static_cast<UINT>(spec.push_constants_byte_size / sizeof(DWORD)),
				},
				.ShaderVisibility = convert_shader_stage(spec.push_constants_visibility),
			});

			SmallList<D3D12_DESCRIPTOR_RANGE1>	 descriptor_ranges;
			SmallList<D3D12_STATIC_SAMPLER_DESC> static_samplers;

			uint8_t index = 0;
			for(auto& layout : spec.layouts)
			{
				if(layout.d3d12.holds_descriptor_table())
				{
					auto ranges			 = layout.d3d12.get_descriptor_ranges();
					auto new_range_begin = descriptor_ranges.insert(descriptor_ranges.end(), ranges.begin(), ranges.end());
					for(auto it = new_range_begin; it != descriptor_ranges.end(); ++it)
						it->RegisterSpace = index;

					parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
						.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
						.DescriptorTable {
							.NumDescriptorRanges = count(ranges),
							.pDescriptorRanges	 = &*new_range_begin,
						},
						.ShaderVisibility = layout.d3d12.get_visibility(),
					});
				}
				else
				{
					auto& new_parameter = parameters.emplace_back(layout.d3d12.get_root_descriptor_parameter());
					new_parameter.Descriptor.RegisterSpace = index;
				}

				auto descs		= layout.d3d12.get_static_sampler_descs();
				auto desc_begin = static_samplers.insert(static_samplers.end(), descs.begin(), descs.end());
				for(auto it = desc_begin; it != static_samplers.end(); ++it)
					it->RegisterSpace = index;

				// Pre-increment to be offset by 1, because we reserve index 0 for push constants.
				parameter_indices.emplace(layout.d3d12.get_id(), ++index);
			}

			D3D12_VERSIONED_ROOT_SIGNATURE_DESC const desc {
				.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
				.Desc_1_1 {
					.NumParameters	   = count(parameters),
					.pParameters	   = parameters.data(),
					.NumStaticSamplers = count(static_samplers),
					.pStaticSamplers   = static_samplers.data(),
					.Flags			   = D3D12_ROOT_SIGNATURE_FLAG_NONE,
				},
			};
			ComUnique<ID3DBlob> blob, error_blob;
			auto result = D3D12SerializeVersionedRootSignature(&desc, std::out_ptr(blob), std::out_ptr(error_blob));
			VT_CHECK_RESULT(result, "Failed to serialize D3D12 root signature.");

			result = device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
												 VT_COM_OUT(root_signature));
			VT_CHECK_RESULT(result, "Failed to create D3D12 root signature.");
		}

		RootSignatureParameterMap const& get_parameter_map() const
		{
			return parameter_indices;
		}

		ID3D12RootSignature* ptr() const
		{
			return root_signature.get();
		}

	private:
		ComUnique<ID3D12RootSignature> root_signature;
		RootSignatureParameterMap	   parameter_indices;
	};
}
