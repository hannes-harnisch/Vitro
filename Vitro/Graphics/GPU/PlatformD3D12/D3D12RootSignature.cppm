module;
#include "D3D12API.hpp"

#include <unordered_map>
export module vt.Graphics.D3D12.RootSignature;

import vt.Graphics.D3D12.Handle;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;

namespace vt::d3d12
{
	struct IndexPair
	{
		uint8_t view_index;
		uint8_t sampler_table_index;
	};

	export class RootParameterMap
	{
	public:
		void	  emplace(unsigned layout_id, uint8_t view_index, uint8_t sampler_table_index);
		IndexPair find(unsigned layout_id) const;

	private:
		std::unordered_map<unsigned, IndexPair> map;
	};

	export class D3D12RootSignature
	{
	public:
		D3D12RootSignature(RootSignatureSpecification const& spec, ID3D12Device4& device);

		RootParameterMap const& get_parameter_map() const;
		ID3D12RootSignature*	get_handle() const;

	private:
		ComUnique<ID3D12RootSignature> root_signature;
		RootParameterMap			   parameter_indices;
	};
}
