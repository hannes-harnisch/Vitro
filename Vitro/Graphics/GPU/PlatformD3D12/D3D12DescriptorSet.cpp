module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
export module vt.Graphics.D3D12.DescriptorSet;

namespace vt::d3d12
{
	export class D3D12DescriptorSet
	{
		friend class D3D12Device;

	public:
		// Use of this constructor indicates that the descriptor set will hold a descriptor table.
		D3D12DescriptorSet(unsigned					   layout_id,
						   D3D12_GPU_DESCRIPTOR_HANDLE view_table_start,
						   D3D12_GPU_DESCRIPTOR_HANDLE sampler_table_start) :
			layout_id(layout_id),
			param_type(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE),
			view_table_start(view_table_start),
			sampler_table_start(sampler_table_start)
		{}

		// Use of this constructor indicates that the descriptor set will hold a root descriptor. Initialization of the GPU
		// virtual address member happens when the descriptor set is updated using the device.
		D3D12DescriptorSet(unsigned layout_id, D3D12_ROOT_PARAMETER_TYPE param_type) :
			layout_id(layout_id), param_type(param_type)
		{}

		unsigned get_layout_id() const
		{
			return layout_id;
		}

		D3D12_ROOT_PARAMETER_TYPE get_parameter_type() const
		{
			return param_type;
		}

		// Returns a zero descriptor if the descriptor set contains no view descriptor table.
		D3D12_GPU_DESCRIPTOR_HANDLE get_view_table_start() const
		{
			assert_descriptor_table();
			return view_table_start;
		}

		// Returns a zero descriptor if the descriptor set contains no sampler descriptor table.
		D3D12_GPU_DESCRIPTOR_HANDLE get_sampler_table_start() const
		{
			assert_descriptor_table();
			return sampler_table_start;
		}

		D3D12_GPU_VIRTUAL_ADDRESS get_gpu_address() const
		{
			VT_ASSERT(param_type != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					  "This method is only valid for descriptor sets holding a root descriptor.");

			return root_descriptor_gpu_address;
		}

	private:
		unsigned				  layout_id;
		D3D12_ROOT_PARAMETER_TYPE param_type;
		union
		{
			struct
			{
				D3D12_GPU_DESCRIPTOR_HANDLE view_table_start;
				D3D12_GPU_DESCRIPTOR_HANDLE sampler_table_start;
			};
			D3D12_GPU_VIRTUAL_ADDRESS root_descriptor_gpu_address;
		};

		void assert_descriptor_table() const
		{
			VT_ASSERT(param_type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
					  "This method is only valid for descriptor sets not holding a root descriptor.");
		}
	};
}
