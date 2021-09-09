module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <vector>
export module vt.D3D12.RootSignature;

import vt.Core.Algorithm;
import vt.Core.Enum;
import vt.D3D12.DescriptorSetLayout;
import vt.D3D12.Utils;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.Device;
import vt.Graphics.RootSignatureInfo;

namespace vt::d3d12
{
	D3D12_ROOT_PARAMETER_TYPE convert_to_root_signature_parameter_type(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{
			case Buffer:
			case ByteAddressBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
			case ReadWriteTexture:
			case ReadWriteBuffer:
			case StructuredBuffer: return D3D12_ROOT_PARAMETER_TYPE_UAV;
			case UniformBuffer: return D3D12_ROOT_PARAMETER_TYPE_CBV;
		}
		throw std::runtime_error(std::format("Descriptor type '{}' cannot be used for root descriptors.", enum_name(type)));
	}

	export class D3D12RootSignature
	{
	public:
		D3D12RootSignature(Device const& device, RootSignatureInfo const& info)
		{
			std::vector<D3D12_ROOT_PARAMETER1> parameters(D3D12_MAX_ROOT_COST);
			parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
				.Constants {
					.ShaderRegister = 0,
					.RegisterSpace	= 0,
					.Num32BitValues = info.push_constants_size_in_32bit_units,
				},
				.ShaderVisibility = convert_shader_stage(info.push_constants_visibility),
			});

			for(auto binding : info.root_descriptor_bindings)
				parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
					.ParameterType = convert_to_root_signature_parameter_type(binding.type),
					.Descriptor {
						.ShaderRegister = binding.slot,
						.RegisterSpace	= 0,
						.Flags			= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
					},
					.ShaderVisibility = convert_shader_stage(binding.visibility),
				});

			for(auto& set_layout : info.descriptor_set_layouts)
				parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
					.ParameterType	  = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					.DescriptorTable  = set_layout.d3d12.get_descriptor_table(),
					.ShaderVisibility = set_layout.d3d12.get_visibility(),
				});

			D3D12_VERSIONED_ROOT_SIGNATURE_DESC const desc {
				.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
				.Desc_1_1 {
					.NumParameters	   = count(parameters),
					.pParameters	   = parameters.data(),
					.NumStaticSamplers = 0,
					.pStaticSamplers   = nullptr,
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

		ID3D12RootSignature* ptr() const
		{
			return root_signature.get();
		}

	private:
		ComUnique<ID3D12RootSignature> root_signature;
	};
}
