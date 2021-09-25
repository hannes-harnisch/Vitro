module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <format>
#include <stdexcept>
#include <unordered_map>
#include <vector>
export module vt.Graphics.D3D12.RootSignature;

import vt.Core.Algorithm;
import vt.Core.Enum;
import vt.Graphics.D3D12.DescriptorSetLayout;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Sampler;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;

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
		D3D12RootSignature(Device& device, RootSignatureSpecification const& spec)
		{
			if(spec.push_constants_byte_size % sizeof(DWORD) != 0)
				throw std::invalid_argument(std::format("Push constants byte size must be divisible by {}.", sizeof(DWORD)));

			std::vector<D3D12_ROOT_PARAMETER1> parameters;
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

			uint8_t index = 1;
			for(auto& layout : spec.layouts)
			{
				parameters.emplace_back(layout.d3d12.get_root_parameter());
				parameter_indices.emplace(layout.d3d12.get_id(), index++);
			}

			std::vector<D3D12_STATIC_SAMPLER_DESC> static_samplers;
			static_samplers.reserve(spec.static_sampler_specs.size());
			for(auto& static_sampler_spec : spec.static_sampler_specs)
				static_samplers.emplace_back(convert_static_sampler_spec(static_sampler_spec));

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
			ID3DBlob *raw_blob, *raw_error;

			auto result = D3D12SerializeVersionedRootSignature(&desc, &raw_blob, &raw_error);

			ComUnique<ID3DBlob> blob(raw_blob), error(raw_error);
			VT_ASSERT_RESULT(result, "Failed to serialize D3D12 root signature.");

			ID3D12RootSignature* raw_root_signature;
			result = device.d3d12.ptr()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
															 IID_PPV_ARGS(&raw_root_signature));
			root_signature.reset(raw_root_signature);
			VT_ASSERT_RESULT(result, "Failed to create D3D12 root signature.");
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
