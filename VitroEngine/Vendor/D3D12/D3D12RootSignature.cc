module;
#include "Core/Macros.hh"
#include "D3D12API.hh"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <vector>
export module vt.D3D12.RootSignature;

import vt.Core.Algorithm;
import vt.Core.Enum;
import vt.D3D12.DescriptorSetLayout;
import vt.D3D12.Handle;
import vt.D3D12.Sampler;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;

namespace stdv = std::views;

namespace vt::d3d12
{
	export class D3D12RootSignature
	{
	public:
		D3D12RootSignature(Device const& device, RootSignatureInfo const& info)
		{
			validate_unique_update_frequencies(info);

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

			auto layout_freqs = stdv::transform(info.layouts, [](DescriptorSetLayout const& layout) {
				return &layout.d3d12;
			});
			std::vector<D3D12DescriptorSetLayout const*> layouts(layout_freqs.begin(), layout_freqs.end());
			std::sort(layouts.begin(), layouts.end(),
					  [](D3D12DescriptorSetLayout const* left, D3D12DescriptorSetLayout const* right) {
						  return left->get_update_frequency() < right->get_update_frequency();
					  });
			for(auto& layout : layouts)
				parameters.emplace_back(layout->get_root_parameter());

			auto static_sampler_infos = stdv::transform(info.static_samplers, [](StaticSamplerInfo const& static_sampler) {
				D3D12Sampler sampler(static_sampler.sampler_info);
				return sampler.get_static_sampler_desc(static_sampler.slot, 0, static_sampler.visibility);
			});
			std::vector<D3D12_STATIC_SAMPLER_DESC> static_samplers(static_sampler_infos.begin(), static_sampler_infos.end());

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

			auto layout_freqs = stdv::transform(info.layouts, [](DescriptorSetLayout const& layout) {
				return layout.d3d12.get_update_frequency();
			});
			freqs.insert(freqs.end(), layout_freqs.begin(), layout_freqs.end());

			std::sort(freqs.begin(), freqs.end());
			bool out_of_range = std::any_of(freqs.begin(), freqs.end(), [](unsigned char freq) {
				return freq == 0 || freq >= D3D12_MAX_ROOT_COST;
			});
			if(out_of_range)
				throw std::invalid_argument("Root signature parameters must have update frequencies between 1 and 64 "
											"(D3D12_MAX_ROOT_COST).");

			auto adjacent = std::adjacent_find(freqs.begin(), freqs.end());
			if(adjacent != freqs.end())
				throw std::invalid_argument("Root signature parameters must not contain duplicate update frequencies.");
		}
	};
}
