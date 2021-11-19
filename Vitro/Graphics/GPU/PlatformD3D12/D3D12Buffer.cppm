module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Buffer;

import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.Resource;

namespace vt::d3d12
{
	export class D3D12Buffer : public Resource
	{
	public:
		D3D12Buffer(BufferSpecification const& spec,
					ID3D12Device4&			   device,
					D3D12MA::Allocator&		   allocator,
					DescriptorPool&			   pool);

	private:
		void initialize_resource(BufferSpecification const& spec, D3D12MA::Allocator& allocator);
		void initialize_descriptor(BufferSpecification const& spec, ID3D12Device4& device, DescriptorPool& pool);
	};
}
