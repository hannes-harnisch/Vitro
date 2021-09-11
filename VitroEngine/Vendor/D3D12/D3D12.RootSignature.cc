module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <vector>
export module vt.D3D12.RootSignature;

import vt.Core.Algorithm;
import vt.Core.Enum;
import vt.D3D12.DescriptorSetLayout;
import vt.D3D12.Sampler;
import vt.D3D12.Utils;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;
import vt.Graphics.RootSignatureInfo;

namespace stdv = std::views;

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
			std::vector<D3D12_ROOT_PARAMETER1> parameters;
			parameters.reserve(D3D12_MAX_ROOT_COST);
			parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
				.Constants {
					.ShaderRegister = 0,
					.RegisterSpace	= 0,
					.Num32BitValues = info.push_constants_size_in_32bit_units,
				},
				.ShaderVisibility = convert_shader_stage(info.push_constants_visibility),
			});
			validate_unique_update_frequencies(info);

			std::vector<RootDescriptorBinding> root_descs(info.root_descs.begin(), info.root_descs.end());
			std::sort(root_descs.begin(), root_descs.end(), [](RootDescriptorBinding left, RootDescriptorBinding right) {
				return left.update_frequency < right.update_frequency;
			});
			for(auto binding : root_descs)
				parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
					.ParameterType = convert_to_root_signature_parameter_type(binding.type),
					.Descriptor {
						.ShaderRegister = binding.slot,
						.RegisterSpace	= 0,
						.Flags			= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
					},
					.ShaderVisibility = convert_shader_stage(binding.visibility),
				});

			auto d3d12_layouts = stdv::transform(info.desc_set_layouts, [](DescriptorSetLayout const& layout) {
				return layout.d3d12;
			});
			std::vector<D3D12DescriptorSetLayout> desc_set_layouts(d3d12_layouts.begin(), d3d12_layouts.end());
			std::sort(desc_set_layouts.begin(), desc_set_layouts.end(),
					  [](D3D12DescriptorSetLayout const& left, D3D12DescriptorSetLayout const& right) {
						  return left.get_update_frequency() < right.get_update_frequency();
					  });
			for(auto& set_layout : desc_set_layouts)
				parameters.emplace_back(D3D12_ROOT_PARAMETER1 {
					.ParameterType	  = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					.DescriptorTable  = set_layout.get_descriptor_table(),
					.ShaderVisibility = set_layout.get_visibility(),
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

		static void validate_unique_update_frequencies(RootSignatureInfo const& info)
		{
			std::vector<unsigned char> freqs;
			freqs.reserve(D3D12_MAX_ROOT_COST);

			auto root_desc_freqs = stdv::transform(info.root_descs, [](RootDescriptorBinding binding) {
				return binding.update_frequency;
			});
			freqs.insert(freqs.end(), root_desc_freqs.begin(), root_desc_freqs.end());

			auto desc_set_freqs = stdv::transform(info.desc_set_layouts, [](DescriptorSetLayout const& layout) {
				return static_cast<unsigned char>(layout.d3d12.get_update_frequency());
			});
			freqs.insert(freqs.end(), desc_set_freqs.begin(), desc_set_freqs.end());

			std::sort(freqs.begin(), freqs.end());
			bool out_of_range = std::any_of(freqs.begin(), freqs.end(), [](unsigned char freq) {
				return freq == 0 || freq >= D3D12_MAX_ROOT_COST;
			});
			if(out_of_range)
				throw std::invalid_argument("Root signature parameters must have update frequencies between 1 and 64 "
											"(D3D12_MAX_ROOT_COST).");
			// Store hash(?) of descriptor set layout?
			auto adjacent = std::adjacent_find(freqs.begin(), freqs.end());
			if(adjacent != freqs.end())
				throw std::invalid_argument("Root signature parameters must not contain duplicate update frequencies.");
		}
	};
}
