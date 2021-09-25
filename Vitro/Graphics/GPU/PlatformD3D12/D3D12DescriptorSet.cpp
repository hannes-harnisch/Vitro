module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.DescriptorSet;

namespace vt::d3d12
{
	export class D3D12DescriptorSet
	{
	public:
		D3D12DescriptorSet()
		{}

		unsigned get_layout_id() const
		{
			return layout_id;
		}

		D3D12_ROOT_PARAMETER_TYPE get_parameter_type() const
		{
			return param_type;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE get_table_handle() const
		{
			VT_ASSERT(param_type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					  "This method is only valid for descriptor sets holding a D3D12 descriptor table.");

			return table_handle;
		}

		D3D12_GPU_VIRTUAL_ADDRESS get_gpu_address() const
		{
			VT_ASSERT(param_type == D3D12_ROOT_PARAMETER_TYPE_CBV || param_type == D3D12_ROOT_PARAMETER_TYPE_SRV ||
						  param_type == D3D12_ROOT_PARAMETER_TYPE_UAV,
					  "This method is only valid for descriptor sets holding a CBV, SRV or UAV descriptor.");

			return root_descriptor_gpu_address;
		}

	private:
		unsigned				  layout_id;
		D3D12_ROOT_PARAMETER_TYPE param_type;
		union
		{
			D3D12_GPU_DESCRIPTOR_HANDLE table_handle;
			D3D12_GPU_VIRTUAL_ADDRESS	root_descriptor_gpu_address;
		};
	};
}
