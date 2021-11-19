module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
export module vt.Graphics.D3D12.DescriptorSet;

import vt.Core.Array;
import vt.Graphics.D3D12.DescriptorSetLayout;

namespace vt::d3d12
{
	export class D3D12DescriptorSet
	{
		friend class D3D12Device;

	public:
		// Use of this constructor indicates that the descriptor set will hold a descriptor table.
		D3D12DescriptorSet(unsigned							  layout_id,
						   ConstSpan<D3D12_DESCRIPTOR_RANGE1> view_ranges,
						   D3D12_GPU_DESCRIPTOR_HANDLE		  sampler_table_start,
						   D3D12_GPU_DESCRIPTOR_HANDLE		  view_table_start);

		// Use of this constructor indicates that the descriptor set will hold a root descriptor. Initialization of the GPU
		// virtual address member happens when the descriptor set is updated using the device.
		D3D12DescriptorSet(unsigned					   layout_id,
						   D3D12_ROOT_PARAMETER_TYPE   view_param_type,
						   D3D12_GPU_DESCRIPTOR_HANDLE sampler_table_start);

		unsigned				  get_layout_id() const;
		D3D12_ROOT_PARAMETER_TYPE get_view_parameter_type() const;
		bool					  holds_root_descriptor() const;

		// Returns a zero descriptor if the descriptor set contains no view descriptor table.
		D3D12_GPU_DESCRIPTOR_HANDLE get_view_table_start() const;

		// Returns a zero descriptor if the descriptor set contains no sampler descriptor table.
		D3D12_GPU_DESCRIPTOR_HANDLE get_sampler_table_start() const;
		D3D12_GPU_VIRTUAL_ADDRESS	get_gpu_address() const;

	private:
		unsigned					   layout_id;
		D3D12_ROOT_PARAMETER_TYPE	   view_param_type : 16;
		unsigned					   range_count	   : 16;
		RangeOffsetMap::const_iterator range_begin;
		D3D12_GPU_DESCRIPTOR_HANDLE	   sampler_table_start;
		union
		{
			D3D12_GPU_DESCRIPTOR_HANDLE view_table_start;
			D3D12_GPU_VIRTUAL_ADDRESS	root_descriptor_gpu_address;
		};
	};
}
