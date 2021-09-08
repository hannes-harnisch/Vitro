module;
#include "D3D12.API.hh"
export module vt.D3D12.DescriptorSet;

namespace vt
{
	export class D3D12DescriptorSet
	{
	public:
	private:
		ID3D12DescriptorHeap* owning_heap;
		unsigned char		  root_parameter_index;
	};
}
