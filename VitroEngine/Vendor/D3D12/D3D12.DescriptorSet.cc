module;
#include "D3D12.API.hh"
export module vt.D3D12.DescriptorSet;

namespace vt
{
	export class D3D12DescriptorSet
	{
	public:
	private:
		unsigned char root_parameter_index;
		bool		  holds_descriptor_table;
		union
		{
			D3D12_GPU_DESCRIPTOR_HANDLE descriptor_table_start;
			D3D12_GPU_VIRTUAL_ADDRESS	root_descriptor_address;
		};
	};
}
