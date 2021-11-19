module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
module vt.Graphics.D3D12.DescriptorSet;

import vt.Core.Array;
import vt.Graphics.D3D12.DescriptorSetLayout;

namespace vt::d3d12
{
	D3D12DescriptorSet::D3D12DescriptorSet(unsigned							  layout_id,
										   ConstSpan<D3D12_DESCRIPTOR_RANGE1> view_ranges,
										   D3D12_GPU_DESCRIPTOR_HANDLE		  sampler_table_start,
										   D3D12_GPU_DESCRIPTOR_HANDLE		  view_table_start) :
		layout_id(layout_id),
		view_param_type(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE),
		sampler_table_start(sampler_table_start),
		view_table_start(view_table_start)
	{}

	D3D12DescriptorSet::D3D12DescriptorSet(unsigned					   layout_id,
										   D3D12_ROOT_PARAMETER_TYPE   view_param_type,
										   D3D12_GPU_DESCRIPTOR_HANDLE sampler_table_start) :
		layout_id(layout_id), view_param_type(view_param_type), sampler_table_start(sampler_table_start)
	{}

	unsigned D3D12DescriptorSet::get_layout_id() const
	{
		return layout_id;
	}

	D3D12_ROOT_PARAMETER_TYPE D3D12DescriptorSet::get_view_parameter_type() const
	{
		return view_param_type;
	}

	bool D3D12DescriptorSet::holds_root_descriptor() const
	{
		return view_param_type != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorSet::get_view_table_start() const
	{
		VT_ASSERT(view_param_type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
				  "This method is only valid for descriptor sets not holding a root descriptor.");

		return view_table_start;
	}

	// Returns a zero descriptor if the descriptor set contains no sampler descriptor table.
	D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorSet::get_sampler_table_start() const
	{
		return sampler_table_start;
	}

	D3D12_GPU_VIRTUAL_ADDRESS D3D12DescriptorSet::get_gpu_address() const
	{
		VT_ASSERT(view_param_type != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
				  "This method is only valid for descriptor sets holding a root descriptor.");

		return root_descriptor_gpu_address;
	}
}
